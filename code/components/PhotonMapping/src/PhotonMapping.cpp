#include "PhotonMapping.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <algorithm>
#include <filesystem>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincodec.h>
#include "server/Server.hpp"

namespace PhotonMapping
{
    using std::vector;

    static inline int nextAxis(int a) { return (a+1)%3; }

    void PhotonKDTree::build(vector<Photon>& photons) {
        photonsRef = &photons;
        nodes.clear();
        vector<int> indices(photons.size());
        for (int i=0;i<(int)indices.size();++i) indices[i]=i;
        root = buildRec(indices, 0, photons);
    }
    int PhotonKDTree::buildRec(vector<int>& indices, int depth, vector<Photon>& photons) {
        if (indices.empty()) return -1;
        int axis = depth % 3;
        size_t mid = indices.size()/2;
        std::nth_element(indices.begin(), indices.begin()+mid, indices.end(), [&](int a,int b){
            return (*photonsRef)[a].position[axis] < (*photonsRef)[b].position[axis];
        });
        int idx = indices[mid];
        vector<int> left(indices.begin(), indices.begin()+mid);
        vector<int> right(indices.begin()+mid+1, indices.end());
        Node n{}; n.index=idx; n.axis=axis; n.left=-1; n.right=-1;
        nodes.push_back(n);
        int nodeIdx = (int)nodes.size()-1;
        int l = buildRec(left, depth+1, photons);
        int r = buildRec(right, depth+1, photons);
        nodes[nodeIdx].left = l;
        nodes[nodeIdx].right = r;
        return nodeIdx;
    }
    vector<pair<float,int>> PhotonKDTree::kNearest(const Vec3& p, int k) const {
        priority_queue<pair<float,int>> heap;
        searchRec(root, p, k, heap);
        vector<pair<float,int>> res; res.reserve(heap.size());
        while(!heap.empty()) { res.push_back(heap.top()); heap.pop(); }
        std::reverse(res.begin(), res.end());
        return res;
    }
    void PhotonKDTree::searchRec(int nodeIdx, const Vec3& p, int k, priority_queue<pair<float,int>>& heap) const {
        if (nodeIdx==-1) return;
        const auto& n = nodes[nodeIdx];
        const auto& ph = (*photonsRef)[n.index];
        float d2 = glm::dot(ph.position - p, ph.position - p);
        if ((int)heap.size() < k) heap.emplace(d2, n.index);
        else if (d2 < heap.top().first) { heap.pop(); heap.emplace(d2, n.index); }
        int axis = n.axis;
        float diff = p[axis] - ph.position[axis];
        int first = diff < 0 ? n.left : n.right;
        int second = diff < 0 ? n.right : n.left;
        searchRec(first, p, k, heap);
        if ((int)heap.size() < k || diff*diff < heap.top().first) searchRec(second, p, k, heap);
    }

    static inline float rrProb(const RGB& a) {
        return (a.r + a.g + a.b) * (1.0f/3.0f);
    }
    static inline RGB clamp01(const RGB& c) {
        return NRenderer::clamp(c);
    }

    RGB PhotonMapperRenderer::gamma(const RGB& rgb) {
        return { sqrt(rgb.r), sqrt(rgb.g), sqrt(rgb.b) };
    }

    RGB PhotonMapperRenderer::getAlbedo(Material& mat) {
        using PW = Property::Wrapper;
        if (auto optAlb = mat.getProperty<PW::RGBType>("albedo")) return clamp01((*optAlb).value);
        if (auto optDif = mat.getProperty<PW::RGBType>("diffuseColor")) return clamp01((*optDif).value);
        return RGB{1,1,1};
    }

    HitRecord PhotonMapperRenderer::closestHitObject(const Ray& r) {
        HitRecord closestHit = getMissRecord();
        float closest = FLOAT_INF;
        for (auto& s : scene.sphereBuffer) {
            auto hit = Intersection::xSphere(r, s, 0.000001f, closest);
            if (hit && hit->t < closest) { closest = hit->t; closestHit = hit; }
        }
        for (auto& t : scene.triangleBuffer) {
            auto hit = Intersection::xTriangle(r, t, 0.000001f, closest);
            if (hit && hit->t < closest) { closest = hit->t; closestHit = hit; }
        }
        for (auto& p : scene.planeBuffer) {
            auto hit = Intersection::xPlane(r, p, 0.000001f, closest);
            if (hit && hit->t < closest) { closest = hit->t; closestHit = hit; }
        }
        return closestHit;
    }

    void PhotonMapperRenderer::buildPhotonMap() {
        photons.clear();
        totalEmitted = {0,0,0};
        totalRecorded = {0,0,0};
        totalEmittedRaw = {0,0,0};
        totalRecordedRaw = {0,0,0};
        emittedCount = 0;
        absorbedCount = 0;
        escapedCount = 0;
        emittedEnergy = {0,0,0};
        absorbedEnergy = {0,0,0};
        escapedEnergy = {0,0,0};
        int photonCountLocal = std::max(1, photonCount);
        vector<float> lightWeights;
        vector<int> lightTypes;
        struct LRef { int type; int idx; };
        vector<LRef> lrefs;
        auto addWeight = [&](float w, int type, int idx){ if (w>0) { lightWeights.push_back(w); lrefs.push_back({type, idx}); } };
        for (int i=0;i<(int)scene.pointLightBuffer.size();++i) {
            const auto& pl = scene.pointLightBuffer[i];
            float w = glm::length(pl.intensity);
            addWeight(w, 0, i);
        }
        for (int i=0;i<(int)scene.areaLightBuffer.size();++i) {
            const auto& a = scene.areaLightBuffer[i];
            float area = glm::length(glm::cross(a.u, a.v));
            float w = glm::length(a.radiance) * area;
            addWeight(w, 1, i);
        }
        float sumW = 0.f; for (auto w : lightWeights) sumW += w;
        if (sumW <= 0.f) return;
        vector<float> cdf(lightWeights.size());
        float acc=0; for (size_t i=0;i<lightWeights.size();++i){ acc+=lightWeights[i]/sumW; cdf[i]=acc; }

        photons.reserve(photonCountLocal);
        int emitted = 0;
        while (emitted < photonCountLocal) {
            float u = glm::linearRand(0.0f, 1.0f);
            size_t choice = std::lower_bound(cdf.begin(), cdf.end(), u) - cdf.begin();
            if (choice >= lrefs.size()) choice = lrefs.size()-1;
            const auto& lr = lrefs[choice];
            Ray ray;
            RGB power{1,1,1};
            RGB powerRaw{1,1,1};
            if (lr.type == 0) {
                const auto& pl = scene.pointLightBuffer[lr.idx];
                ray.origin = pl.position;
                Vec3 dir = glm::sphericalRand(1.0f);
                ray.setDirection(dir);
                float w = lightWeights[choice];
                float expected = float(photonCountLocal) * (w / sumW);
                expected = std::max(expected, 1.0f);
                powerRaw = pl.intensity / expected;
                power = powerRaw;
            } else {
                const auto& a = scene.areaLightBuffer[lr.idx];
                float su = glm::linearRand(0.0f, 1.0f);
                float sv = glm::linearRand(0.0f, 1.0f);
                Vec3 pL = a.position + a.u * su + a.v * sv;
                Vec3 n = glm::normalize(glm::cross(a.u, a.v));
                ray.origin = pL + n*0.001f;
                float r1 = glm::linearRand(0.0f, 1.0f);
                float r2 = glm::linearRand(0.0f, 1.0f);
                float phi = 2.0f * 3.1415926f * r1;
                float cosTheta = sqrt(1.0f - r2);
                float sinTheta = sqrt(r2);
                Vec3 tangent = glm::normalize(a.u);
                Vec3 bitangent = glm::normalize(a.v);
                Vec3 dir = glm::normalize(tangent * (cos(phi)*sinTheta) + bitangent * (sin(phi)*sinTheta) + n * cosTheta);
                ray.setDirection(dir);
                float area = glm::length(glm::cross(a.u, a.v));
                float w = lightWeights[choice];
                float expected = float(photonCountLocal) * (w / sumW);
                expected = std::max(expected, 1.0f);
                powerRaw = a.radiance * area * 3.1415926f / expected;
                power = powerRaw;
            }

            totalEmitted += power;
            totalEmittedRaw += powerRaw;
            emittedCount += 1;
            emittedEnergy += powerRaw;

            RGB throughput = power;
            RGB throughputRaw = powerRaw;
            int termType = 0; // 0=max_bounces, 1=escaped, 2=absorbed

            for (int bounce=0; bounce<maxBounces; ++bounce) {
                auto hit = closestHitObject(ray);
                if (!hit) { 
                    termType = 1; 
                    break; 
                }
                
                Vec3 x = hit->hitPoint;
                Vec3 n = glm::normalize(hit->normal);
                auto& mat = scene.materials[hit->material.index()];
                
                auto getRGB = [&](const std::string& key, const RGB& def)->RGB{
                    auto p = mat.getProperty<Property::Wrapper::RGBType>(key);
                    return p ? clamp01(p->value) : def;
                };
                auto getFloat = [&](const std::string& key, float def)->float{
                    auto p = mat.getProperty<Property::Wrapper::FloatType>(key);
                    return p ? p->value : def;
                };
                
                bool isMirror = (mat.type == 3) || mat.hasProperty("reflect");
                bool isGlass  = (mat.type == 2) || mat.hasProperty("ior");
                
                if (isMirror) {
                    RGB reflectColor = getRGB("reflect", RGB{1,1,1});
                    // 只更新throughput，不累加absorbedEnergy
                    throughput = throughput * reflectColor;
                    throughputRaw = RGB{ 
                        throughputRaw.r * reflectColor.r, 
                        throughputRaw.g * reflectColor.g, 
                        throughputRaw.b * reflectColor.b 
                    };
                    Vec3 Rdir = glm::normalize(glm::reflect(ray.direction, n));
                    ray = Ray{ x + n*0.001f, Rdir };
                    continue;
                    
                } else if (isGlass) {
                    float ior = getFloat("ior", 1.5f);
                    bool front = glm::dot(ray.direction, n) < 0.f;
                    Vec3 Nf = front ? n : -n;
                    float eta = front ? (1.0f/ior) : ior;
                    Vec3 Tdir = glm::refract(glm::normalize(ray.direction), Nf, eta);
                    
                    if (glm::length(Tdir) > 0.f) {
                        RGB absorbed = getRGB("absorbed", RGB{1,1,1});
                        // 只更新throughput，不累加absorbedEnergy
                        throughput = throughput * absorbed;
                        throughputRaw = RGB{ 
                            throughputRaw.r * absorbed.r, 
                            throughputRaw.g * absorbed.g, 
                            throughputRaw.b * absorbed.b 
                        };
                        ray = Ray{ x - Nf*0.001f, glm::normalize(Tdir) };
                    } else {
                        Vec3 Rdir = glm::normalize(glm::reflect(ray.direction, n));
                        ray = Ray{ x + n*0.001f, Rdir };
                    }
                    continue;
                    
                } else {
                    // 漫反射表面
                    RGB albedo = getAlbedo(mat);
                    float p = glm::clamp(rrProb(albedo), 0.05f, 0.95f);
                    float uRR = glm::linearRand(0.0f, 1.0f);
                    
                    // Russian Roulette 判断
                    if (uRR > p) {
                        // 被吸收，终止追踪
                        termType = 2;
                        break;
                    }
                    
                    // 更新throughput
                    throughput = throughput * (albedo / p);
                    throughputRaw = RGB{ 
                        throughputRaw.r * albedo.r / p, 
                        throughputRaw.g * albedo.g / p, 
                        throughputRaw.b * albedo.b / p 
                    };
                    
                    // 存储光子
                    photons.push_back(Photon{ x, -ray.direction, throughput });
                    totalRecorded += throughput;
                    totalRecordedRaw += throughputRaw;
                    
                    // 生成新方向
                    Vec3 w = n;
                    Vec3 tangent = glm::normalize(
                        glm::abs(w.x) > 0.9f ? Vec3{0,1,0} : glm::cross(w, Vec3{1,0,0})
                    );
                    Vec3 bitangent = glm::cross(w, tangent);
                    
                    float r1 = glm::linearRand(0.0f, 1.0f);
                    float r2 = glm::linearRand(0.0f, 1.0f);
                    float phi = 2.0f * 3.1415926f * r1;
                    float cosTheta = sqrt(1.0f - r2);
                    float sinTheta = sqrt(r2);
                    
                    Vec3 dir = glm::normalize(
                        tangent * (cos(phi)*sinTheta) + 
                        bitangent * (sin(phi)*sinTheta) + 
                        w * cosTheta
                    );
                    ray = Ray{ x + n*0.001f, dir };
                }
            }

            // 统计最终能量去向
            if (termType == 1) {
                // 逃逸出场景
                escapedCount += 1;
                escapedEnergy += throughputRaw;
            } else {
                // 被吸收（包括 RR 终止和达到最大弹跳次数）
                absorbedCount += 1;
                absorbedEnergy += throughputRaw;
            }

            emitted++;
        }
        photonTree = std::make_unique<PhotonKDTree>();
        photonTree->build(photons);
    }

    RGB PhotonMapperRenderer::trace(const Ray& r, int currDepth) {
        if (currDepth > (int)scene.renderOption.depth) {
            return scene.ambient.constant;
        }
        auto [lightT, lightRadiance] = closestHitLight(r);
        auto hit = closestHitObject(r);
        if (!hit) {
            if (lightT < FLOAT_INF) return lightRadiance;
            if (scene.ambient.type == NRenderer::Ambient::Type::CONSTANT) return scene.ambient.constant;
            return RGB{0,0,0};
        }
        if (lightT < hit->t) {
            return lightRadiance;
        }
        auto& mat = scene.materials[hit->material.index()];
        bool isMirror = (mat.type == 3) || mat.hasProperty("reflect");
        bool isGlass  = (mat.type == 2) || mat.hasProperty("ior");
        Vec3 x = hit->hitPoint;
        Vec3 n = glm::normalize(hit->normal);
        if (isMirror) {
            Vec3 Rdir = glm::normalize(glm::reflect(r.direction, n));
            return trace(Ray{ x + n*0.001f, Rdir }, currDepth + 1);
        }
        if (isGlass) {
            float ior = 1.5f;
            if (auto p = mat.getProperty<Property::Wrapper::FloatType>("ior")) ior = p->value;
            bool front = glm::dot(r.direction, n) < 0.f;
            Vec3 Nf = front ? n : -n;
            float eta = front ? (1.0f/ior) : ior;
            Vec3 Tdir = glm::refract(glm::normalize(r.direction), Nf, eta);
            if (glm::length(Tdir) > 0.f) {
                return trace(Ray{ x - Nf*0.001f, glm::normalize(Tdir) }, currDepth + 1);
            } else {
                Vec3 Rdir = glm::normalize(glm::reflect(r.direction, n));
                return trace(Ray{ x + n*0.001f, Rdir }, currDepth + 1);
            }
        }
        RGB albedo = getAlbedo(mat);
        auto knn = photonTree ? photonTree->kNearest(hit->hitPoint, gatherK) : vector<pair<float,int>>{};
        if (knn.empty()) return RGB{0,0,0};
        float r2 = knn.back().first;
        RGB sumP{0,0,0};
        for (auto& pr : knn) { sumP += photons[pr.second].power; }
        float area = 3.1415926f * r2;
        RGB L = (albedo / 3.1415926f) * (sumP / area);
        return L;
    }

    auto PhotonMapperRenderer::render() -> RenderResult {
        VertexTransformer vt; vt.exec(spScene);
        buildPhotonMap();
        if (scene.renderOption.visualizePhotonMap) {
            savePhase1PNG("c:\\Users\\17372\\Desktop\\CG\\nrenderer-comment-master\\doc");
        }
        getServer().logger.log(
            std::string("Photon Mapping: ") + std::to_string(photonCount) +  " KNN: " + std::to_string(gatherK) + "N"
        );
        float eSum = totalEmitted.r + totalEmitted.g + totalEmitted.b;
        float rSum = totalRecorded.r + totalRecorded.g + totalRecorded.b;
        float ratio = eSum > 0 ? (rSum / eSum) : 0.0f;
        float eSumRaw = totalEmittedRaw.r + totalEmittedRaw.g + totalEmittedRaw.b;
        float rSumRaw = totalRecordedRaw.r + totalRecordedRaw.g + totalRecordedRaw.b;
        float ratioRaw = eSumRaw > 0 ? (rSumRaw / eSumRaw) : 0.0f;

        float eCount = float(emittedCount);
        float arCount = float(absorbedCount + escapedCount);
        float countRatio = eCount > 0 ? (arCount / eCount) : 0.0f;
        float eEnergySum = emittedEnergy.r + emittedEnergy.g + emittedEnergy.b;
        float arEnergySum = absorbedEnergy.r + absorbedEnergy.g + absorbedEnergy.b;
        float escEnergySum = escapedEnergy.r + escapedEnergy.g + escapedEnergy.b;
        float energyRatio = eEnergySum > 0 ? ((arEnergySum + escEnergySum) / eEnergySum) : 0.0f;
        getServer().logger.log(
            std::string("\n Emitted count=") + std::to_string(emittedCount) +
            " energy=" + std::to_string(emittedEnergy.r) + "," + std::to_string(emittedEnergy.g) + "," + std::to_string(emittedEnergy.b) +
            "\n Absorbed count=" + std::to_string(absorbedCount) +
            " energy=" + std::to_string(absorbedEnergy.r) + "," + std::to_string(absorbedEnergy.g) + "," + std::to_string(absorbedEnergy.b) +
            "\n Escaped count=" + std::to_string(escapedCount) +
            " energy=" + std::to_string(escapedEnergy.r) + "," + std::to_string(escapedEnergy.g) + "," + std::to_string(escapedEnergy.b) +
            "\n count_ratio(absorbed+escaped / emitted)=" + std::to_string(countRatio) +
            "\n energy_ratio(absorbed+escaped / emitted)=" + std::to_string(energyRatio)
        );
        auto pixels = new RGBA[width*height];
        for (unsigned int i=0;i<height;++i) {
            for (unsigned int j=0;j<width;++j) {
                auto ray = camera.shoot(float(j)/float(width), float(i)/float(height));
                RGB c = clamp(trace(ray, 0));
                c = gamma(c);
                pixels[i*width + j] = RGBA{ c.r, c.g, c.b, 1.0f };
            }
        }
        return {pixels, width, height};
    }

    void PhotonMapperRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r; delete[] p;
    }

    void PhotonMapperRenderer::savePhase1PNG(const std::string& dir) {
        std::vector<float> accum(width*height*3, 0.0f);
        std::vector<int> hits(width*height, 0);
        auto& cam = scene.camera;
        glm::mat4 view = glm::lookAt(glm::vec3(cam.position.x, cam.position.y, cam.position.z),
                                      glm::vec3(cam.lookAt.x, cam.lookAt.y, cam.lookAt.z),
                                      glm::vec3(cam.up.x, cam.up.y, cam.up.z));
        float fovRad = glm::radians(cam.fov);
        glm::mat4 proj = glm::perspective(fovRad, float(width)/float(height), 0.1f, 1000.0f);
        for (const auto& ph : photons) {
            Vec3 d = glm::normalize(ph.position - cam.position);
            Ray vray{ cam.position, d };
            auto vhit = closestHitObject(vray);
            float tClosest = FLOAT_INF;
            if (vhit) tClosest = std::min(tClosest, vhit->t);
            for (auto& a : scene.areaLightBuffer) {
                auto ah = Intersection::xAreaLight(vray, a, 0.000001f, tClosest);
                if (ah && ah->t < tClosest) tClosest = ah->t;
            }
            float dist = glm::length(ph.position - cam.position);
            if (!(tClosest < FLOAT_INF) || std::abs(tClosest - dist) > 0.01f) continue;
            glm::vec4 wp(ph.position.x, ph.position.y, ph.position.z, 1.0f);
            glm::vec4 cp = proj * view * wp;
            if (cp.w == 0.0f) continue;
            glm::vec3 ndc = glm::vec3(cp) / cp.w;
            float sx = (ndc.x * 0.5f + 0.5f) * float(width);
            float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * float(height);
            int j = int(sx);
            int i = int(sy);
            if (j < 0 || j >= int(width) || i < 0 || i >= int(height)) continue;
            int idx3 = (i*width + j) * 3;
            accum[idx3+0] += ph.power.r;
            accum[idx3+1] += ph.power.g;
            accum[idx3+2] += ph.power.b;
            hits[i*width + j] += 1;
        }
        double sumLog = 0.0;
        int valid = 0;
        for (unsigned int i=0;i<height;++i) {
            for (unsigned int j=0;j<width;++j) {
                int idx3 = (i*width + j) * 3;
                float Y = 0.2126f*accum[idx3+0] + 0.7152f*accum[idx3+1] + 0.0722f*accum[idx3+2];
                if (Y > 0.0f) { sumLog += std::log(1e-4 + Y); valid++; }
            }
        }
        float logAvg = valid > 0 ? static_cast<float>(std::exp(sumLog / double(valid))) : 1.0f;
        float key = 0.18f;
        float alpha = 0.3f;
        std::vector<uint8_t> out(width*height*4, 255);
        for (unsigned int i=0;i<height;++i) {
            for (unsigned int j=0;j<width;++j) {
                int idx3 = (i*width + j) * 3;
                float R = accum[idx3+0];
                float G = accum[idx3+1];
                float B = accum[idx3+2];
                float Y = 0.2126f*R + 0.7152f*G + 0.0722f*B;
                float L = (key / logAvg) * Y;
                float Lm = L / (1.0f + L);
                float s = (Y > 0.0f) ? (Lm / Y) : 0.0f;
                R *= s; G *= s; B *= s;
                float boost = 1.0f + alpha * std::log(1.0f + float(hits[i*width + j]));
                R *= boost; G *= boost; B *= boost;
                R = std::sqrt(std::min(1.0f, std::max(0.0f, R)));
                G = std::sqrt(std::min(1.0f, std::max(0.0f, G)));
                B = std::sqrt(std::min(1.0f, std::max(0.0f, B)));
                int o = (i*width + j) * 4;
                out[o+0] = static_cast<uint8_t>(R * 255.0f);
                out[o+1] = static_cast<uint8_t>(G * 255.0f);
                out[o+2] = static_cast<uint8_t>(B * 255.0f);
                out[o+3] = 255;
            }
        }
        std::filesystem::path pdir(dir);
        std::filesystem::create_directories(pdir);
        auto path = (pdir / "photon_phase.png");
        std::wstring wpath = path.wstring();
        HRESULT hr = CoInitialize(NULL);
        IWICImagingFactory* factory = NULL;
        IWICStream* stream = NULL;
        IWICBitmapEncoder* encoder = NULL;
        IWICBitmapFrameEncode* frame = NULL;
        IPropertyBag2* bag = NULL;
        bool ok = false;
        if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) {
            if (SUCCEEDED(factory->CreateStream(&stream)) && SUCCEEDED(stream->InitializeFromFilename(wpath.c_str(), GENERIC_WRITE))) {
                if (SUCCEEDED(factory->CreateEncoder(GUID_ContainerFormatPng, NULL, &encoder)) && SUCCEEDED(encoder->Initialize(stream, WICBitmapEncoderNoCache))) {
                    if (SUCCEEDED(encoder->CreateNewFrame(&frame, &bag)) && SUCCEEDED(frame->Initialize(bag))) {
                        if (SUCCEEDED(frame->SetSize(width, height))) {
                            GUID fmt = GUID_WICPixelFormat32bppRGBA;
                            if (SUCCEEDED(frame->SetPixelFormat(&fmt))) {
                                if (SUCCEEDED(frame->WritePixels(height, width*4, (UINT)(out.size()), out.data())) && SUCCEEDED(frame->Commit()) && SUCCEEDED(encoder->Commit())) {
                                    ok = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (bag) bag->Release();
        if (frame) frame->Release();
        if (encoder) encoder->Release();
        if (stream) stream->Release();
        if (factory) factory->Release();
        CoUninitialize();
        if (ok) {
            getServer().logger.log(std::string("Saved phase PNG: ") + path.string());
        } else {
            getServer().logger.log(std::string("Failed to save phase PNG: ") + path.string());
        }
    }
}

#include "server/Server.hpp"

#include "RayTracer.hpp"

#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
#include <algorithm>
#include <cmath>
#include <thread>

namespace RayTracer
{
    /** Gamma 校正 */
    RGB RayTracerRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    /** 渲染任务（多线程） */
    void RayTracerRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        for(int i=off; i<height; i+=step) {
            for (int j=0; j<width; j++) {
                // 单样本：使用像素中心生成主光线
                float x = (float(j) + 0.5f) / float(width);
                float y = (float(i) + 0.5f) / float(height);
                auto ray = camera.shoot(x, y);
                Vec3 color = trace(ray, 0);
                color = gamma(color);
                pixels[(height-i-1)*width+j] = {color, 1};
            }
        }
    }

    /** 执行渲染 */
    auto RayTracerRenderer::render() -> RenderResult {

        // 分配像素缓冲
        RGBA* pixels = new RGBA[width*height]{};

        // 顶点变换刷新对象缓存
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // 构建加速结构（KD-Tree）
        buildAcceleration();

        // 多线程渲染
        const auto taskNums = 8;
        thread t[taskNums];
        for (int i=0; i < taskNums; i++) {
            t[i] = thread(&RayTracerRenderer::renderTask, this, pixels, width, height, i, taskNums);
        }
        for (int i=0; i < taskNums; i++) {
            t[i].join();
        }
        getServer().logger.log("Done...");
        return {pixels, width, height};
    }

    /** 释放渲染结果内存 */
    void RayTracerRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    /** 物体最近相交 */
    HitRecord RayTracerRenderer::closestHitObject(const Ray& r) {
        // 若启用KD-Tree，则优先使用加速结构查询（包含三角形/球体）
        HitRecord kdHit = getMissRecord();
        float kdClosest = FLOAT_INF;
        if (useKDTree && kdTree) {
            kdHit = kdTree->intersect(r, scene, FLOAT_INF);
            if (kdHit) kdClosest = kdHit->t;
        }

        HitRecord closestHit = kdHit;
        float closest = kdClosest;

        // 始终对平面做补充求交（平面不加入KD-Tree）
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.000001f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        // 若未启用KD-Tree，则线性遍历所有物体
        if (!useKDTree || !kdTree) {
            // 球
            for (auto& s : scene.sphereBuffer) {
                auto hitRecord = Intersection::xSphere(r, s, 0.000001f, closest);
                if (hitRecord && hitRecord->t < closest) {
                    closest = hitRecord->t;
                    closestHit = hitRecord;
                }
            }
            // 三角形
            for (auto& t : scene.triangleBuffer) {
                auto hitRecord = Intersection::xTriangle(r, t, 0.000001f, closest);
                if (hitRecord && hitRecord->t < closest) {
                    closest = hitRecord->t;
                    closestHit = hitRecord;
                }
            }
        }
        return closestHit;
    }

    /** 光源最近相交 */
    tuple<float, Vec3> RayTracerRenderer::closestHitLight(const Ray& r) {
        Vec3 v = {};
        HitRecord closest = getHitRecord(FLOAT_INF, {}, {}, {});

        for (auto& a : scene.areaLightBuffer) {
            auto hitRecord = Intersection::xAreaLight(r, a, 0.000001, closest->t);
            if (hitRecord && closest->t > hitRecord->t) {
                closest = hitRecord;
                v = a.radiance;
            }
        }
        return { closest->t, v };
    }

    /** Whitted 风格递归：直射光 + 阴影 + 镜面/折射递归 */
    RGB RayTracerRenderer::trace(const Ray& r, int currDepth) {
        if (currDepth >= depth) {
            return scene.ambient.constant;
        }

        // 直接可见的面光源处理：若光线命中面光源并且其距离小于最近物体，则直接返回其辐射度
        auto [lightT, lightRadiance] = closestHitLight(r);
        auto hitOpt = closestHitObject(r);
        if (!hitOpt) {
            if (lightT < FLOAT_INF) {
                return lightRadiance;
            }
            return Vec3{0};
        }
        if (lightT < hitOpt->t) {
            return lightRadiance;
        }

        const auto& rec = *hitOpt;
        const Vec3 hitPoint = rec.hitPoint;
        const Vec3 N = glm::normalize(rec.normal);
        const Vec3 V = glm::normalize(-r.direction);

        RGB result = (scene.ambient.type == NRenderer::Ambient::Type::CONSTANT)
            ? scene.ambient.constant : RGB{0};

        auto& mat = scene.materials[rec.material.index()];
        auto getRGB = [&](const std::string& key, const RGB& def)->RGB{
            auto p = mat.getProperty<Property::Wrapper::RGBType>(key);
            return p ? p->value : def;
        };
        auto getFloat = [&](const std::string& key, float def)->float{
            auto p = mat.getProperty<Property::Wrapper::FloatType>(key);
            return p ? p->value : def;
        };

        RGB diffuseColor = getRGB("diffuseColor", RGB{1,1,1});
        RGB specularColor = getRGB("specularColor", RGB{0,0,0});
        float specularEx = getFloat("specularEx", 32.0f);

        auto visibleToPointLight = [&](const Vec3& Ldir, float maxDistance)->bool{
            Ray sray{ hitPoint + N*0.001f, Ldir };
            auto sh = closestHitObject(sray);
            return !(sh && sh->t <= maxDistance - 1e-4f);
        };
        auto visibleToDirectional = [&](const Vec3& Ldir)->bool{
            Ray sray{ hitPoint + N*0.001f, Ldir };
            auto sh = closestHitObject(sray);
            return !sh;
        };

        for (const auto& l : scene.pointLightBuffer) {
            Vec3 L = glm::normalize(l.position - hitPoint);
            float NdotL = glm::dot(N, L);
            if (NdotL <= 0.f) continue;
            float distance = glm::length(l.position - hitPoint);
            if (!visibleToPointLight(L, distance)) continue;

            RGB diffuse = (diffuseColor / glm::pi<float>()) * NdotL;
            Vec3 R = glm::reflect(-L, N);
            float specTerm = std::pow(std::max(0.0f, glm::dot(R, V)), specularEx);
            RGB spec = specularColor * specTerm;

            result += (diffuse + spec) * l.intensity;
        }

        for (const auto& l : scene.directionalLightBuffer) {
            Vec3 L = glm::normalize(-l.direction);
            float NdotL = glm::dot(N, L);
            if (NdotL <= 0.f) continue;
            if (!visibleToDirectional(L)) continue;

            RGB diffuse = (diffuseColor / glm::pi<float>()) * NdotL;
            Vec3 R = glm::reflect(-L, N);
            float specTerm = std::pow(std::max(0.0f, glm::dot(R, V)), specularEx);
            RGB spec = specularColor * specTerm;

            result += (diffuse + spec) * l.irradiance;
        }

        // 面光源（简单均匀采样，默认 samples/4 次，至少1次）
        for (const auto& a : scene.areaLightBuffer) {
            int areaSamples = std::max(1u, samples / 4u);
            float area = glm::length(glm::cross(a.u, a.v));
            Vec3 Ln = glm::normalize(glm::cross(a.u, a.v)); // 发光面法线

            RGB accum{0};
            for (int k = 0; k < areaSamples; ++k) {
                float su = glm::linearRand(0.0f, 1.0f);
                float sv = glm::linearRand(0.0f, 1.0f);
                Vec3 pL = a.position + a.u * su + a.v * sv; // 采样点
                Vec3 Lvec = pL - hitPoint;
                float dist = glm::length(Lvec);
                if (dist <= 1e-6f) continue;
                Vec3 Ldir = Lvec / dist;

                float NdotL = glm::dot(N, Ldir);
                if (NdotL <= 0.f) continue;

                // 发光面朝向点的角度
                float cosLight = glm::dot(Ln, -Ldir);
                if (cosLight <= 0.f) continue;

                // 阴影检测（可见性）
                Ray sray{ hitPoint + N*0.001f, Ldir };
                auto sh = closestHitObject(sray);
                if (sh && sh->t <= dist - 1e-4f) continue; // 被遮挡

                RGB diffuse = (diffuseColor / glm::pi<float>()) * NdotL;
                Vec3 R = glm::reflect(-Ldir, N);
                float specTerm = std::pow(std::max(0.0f, glm::dot(R, V)), specularEx);
                RGB spec = specularColor * specTerm;

                float geom = (cosLight * area) / (dist * dist);
                accum += (diffuse + spec) * a.radiance * geom;
            }
            result += accum / float(areaSamples);
        }

        bool isConductor = (mat.type == 3) || mat.hasProperty("reflect");
        if (isConductor && currDepth + 1 <= depth) {
            RGB reflectColor = getRGB("reflect", RGB{1,1,1});
            Vec3 Rdir = glm::normalize(glm::reflect(r.direction, N));
            RGB rc = trace(Ray{ hitPoint + N*0.001f, Rdir }, currDepth + 1);
            result += reflectColor * rc;
        }

        if (mat.type == 2 && currDepth + 1 <= depth) {
            float ior = getFloat("ior", 1.5f);
            bool front = glm::dot(r.direction, N) < 0.f;
            Vec3 Nf = front ? N : -N;
            float eta = front ? (1.0f/ior) : ior;
            Vec3 Tdir = glm::refract(glm::normalize(r.direction), Nf, eta);
            if (glm::length(Tdir) > 0.f) {
                RGB tc = trace(Ray{ hitPoint - Nf*0.001f, glm::normalize(Tdir) }, currDepth + 1);
                RGB absorbed = getRGB("absorbed", RGB{1,1,1});
                result += absorbed * tc;
            } else {
                Vec3 Rdir = glm::normalize(glm::reflect(r.direction, N));
                RGB rc = trace(Ray{ hitPoint + N*0.001f, Rdir }, currDepth + 1);
                result += rc;
            }
        }

        return result;
    }

    /** 构建加速结构 */
    void RayTracerRenderer::buildAcceleration() {
        // 读取开关（仅 RayTracing 使用）
        useKDTree = scene.renderOption.useKDTree;
        if (useKDTree) {
            kdTree = std::make_unique<KDTree>(scene, 8);
        } else {
            kdTree.reset();
        }
    }
}
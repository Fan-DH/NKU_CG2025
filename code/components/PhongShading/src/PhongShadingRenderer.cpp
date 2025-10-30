// ����Ͷ����Ⱦ��ʵ��
// ʵ���˻����Ĺ���Ͷ����Ⱦ�㷨��������Ӱ����
#include "PhongShadingRenderer.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"
#include <algorithm>
#include <cmath>

namespace PhongShading
{
    using namespace std;

    // �ͷ���Ⱦ����е����������ڴ�
    void PhongShadingRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    // Gamma У������ RGB ÿ��ͨ����ƽ��
    RGB PhongShadingRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    // ����λ�������������ڽ�ƥ�䣩�����̶����ȶ�λ�ý�����ɢ��
    struct QKey { int x; int y; int z; };
    struct QKeyHash {
        size_t operator()(const QKey& k) const noexcept {
            // �򵥹�ϣ���
            return (static_cast<size_t>(k.x) * 73856093u) ^
                   (static_cast<size_t>(k.y) * 19349663u) ^
                   (static_cast<size_t>(k.z) * 83492791u);
        }
    };
    struct QKeyEq {
        bool operator()(const QKey& a, const QKey& b) const noexcept {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        }
    };

    static inline QKey quantize(const Vec3& p, float scale = 10000.f) {
        return QKey{
            int(std::floor(p.x * scale + 0.5f)),
            int(std::floor(p.y * scale + 0.5f)),
            int(std::floor(p.z * scale + 0.5f))
        };
    }

    // ���������εĶ��㷨�ߣ�λ������ + �ڽ��ۼ� + ��һ��
    void PhongShadingRenderer::computeTriangleVertexNormals() {
        // �ۼ�ÿ������������淨��
        unordered_map<QKey, Vec3, QKeyHash, QKeyEq> accum;

        // Ԥ����ÿ�������ε��淨��
        vector<Vec3> faceNormals;
        faceNormals.reserve(scene.triangleBuffer.size());
        for (const auto& t : scene.triangleBuffer) {
            Vec3 e1 = t.v2 - t.v1;
            Vec3 e2 = t.v3 - t.v1;
            Vec3 n = glm::normalize(glm::cross(e1, e2));
            faceNormals.push_back(n);
        }

        // ���淨���ۼӵ����������
        for (size_t i = 0; i < scene.triangleBuffer.size(); ++i) {
            const auto& tri = scene.triangleBuffer[i];
            const auto& fn  = faceNormals[i];
            QKey k1 = quantize(tri.v1);
            QKey k2 = quantize(tri.v2);
            QKey k3 = quantize(tri.v3);
            accum[k1] = accum.contains(k1) ? accum[k1] + fn : fn;
            accum[k2] = accum.contains(k2) ? accum[k2] + fn : fn;
            accum[k3] = accum.contains(k3) ? accum[k3] + fn : fn;
        }

        // ��һ���õ�ÿ����������ķ���
        unordered_map<QKey, Vec3, QKeyHash, QKeyEq> vertexNormal;
        vertexNormal.reserve(accum.size());
        for (const auto& [k, v] : accum) {
            vertexNormal.emplace(k, glm::normalize(v));
        }

        // Ϊÿ����������ȡ 3 �����㷨��
        triVertexNormals.resize(scene.triangleBuffer.size());
        for (size_t i = 0; i < scene.triangleBuffer.size(); ++i) {
            const auto& tri = scene.triangleBuffer[i];
            auto k1 = quantize(tri.v1);
            auto k2 = quantize(tri.v2);
            auto k3 = quantize(tri.v3);
            Vec3 n1 = vertexNormal.contains(k1) ? vertexNormal[k1] : faceNormals[i];
            Vec3 n2 = vertexNormal.contains(k2) ? vertexNormal[k2] : faceNormals[i];
            Vec3 n3 = vertexNormal.contains(k3) ? vertexNormal[k3] : faceNormals[i];
            triVertexNormals[i] = { n1, n2, n3 };
        }
    }

    // ������Ⱦ���
    auto PhongShadingRenderer::render() -> RenderResult {
        auto width = scene.renderOption.width;
        auto height = scene.renderOption.height;
        auto pixels = new RGBA[width * height];

        // ��������任������ռ�
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // ���������ζ��㷨�ߣ�����ƽ����ɫ��
        computeTriangleVertexNormals();

        // ����������ɫ������
        ShaderCreator shaderCreator{};
        shaderPrograms.clear();
        shaderPrograms.reserve(scene.materials.size());
        for (auto& mtl : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(mtl, scene.textures));
        }

        // �������ؽ��й���׷��
        for (int i = 0; i < static_cast<int>(height); ++i) {
            for (int j = 0; j < static_cast<int>(width); ++j) {
                // �����������
                auto ray = camera.shoot(float(j) / float(width), float(i) / float(height));
                // ׷�ٻ�ȡ��ɫ
                auto color = trace(ray);
                // ɫ�������� Gamma У��
                color = clamp(color);
                color = gamma(color);
                pixels[(height - i - 1) * width + j] = { color, 1 };
            }
        }

        return { pixels, width, height };
    }

    // ����׷��
    RGB PhongShadingRenderer::trace(const Ray& r) {
        // �޵��Դ�򷵻غ�ɫ
        if (scene.pointLightBuffer.empty()) return { 0, 0, 0 };

        // ��ʹ�õ�һ�����Դ����ʵ�֣�
        auto& l = scene.pointLightBuffer[0];

        // �������
        lastHitTriangleIndex = -1;
        auto closestHitObj = closestHit(r);

        if (closestHitObj) {
            auto& hitRec = *closestHitObj;

            // ��Դ����
            auto out = glm::normalize(l.position - hitRec.hitPoint);

            // �����η��߲�ֵ�������������Σ�
            Vec3 shadingNormal = hitRec.normal;
            if (lastHitTriangleIndex >= 0 && lastHitTriangleIndex < static_cast<int>(scene.triangleBuffer.size())) {
                const auto& tri = scene.triangleBuffer[lastHitTriangleIndex];
                const auto& nset = triVertexNormals[lastHitTriangleIndex];
                // �����������꣨barycentric��
                Vec3 a = tri.v1, b = tri.v2, c = tri.v3, p = hitRec.hitPoint;
                Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
                float d00 = glm::dot(v0, v0);
                float d01 = glm::dot(v0, v1);
                float d11 = glm::dot(v1, v1);
                float d20 = glm::dot(v2, v0);
                float d21 = glm::dot(v2, v1);
                float denom = d00 * d11 - d01 * d01;
                if (fabs(denom) > 1e-8f) {
                    float invDenom = 1.0f / denom;
                    float v = (d11 * d20 - d01 * d21) * invDenom;
                    float w = (d00 * d21 - d01 * d20) * invDenom;
                    float u = 1.0f - v - w;
                    shadingNormal = glm::normalize(nset[0] * u + nset[1] * v + nset[2] * w);
                } else {
                    // �˻������Σ�ʹ���淨��
                    shadingNormal = glm::normalize(glm::cross(b - a, c - a));
                }
            }

            // ���治�ܹ�: ʹ�ò�ֵ�����淨
            if (glm::dot(out, shadingNormal) < 0) {
                return { 0, 0, 0 };
            }

            // ��Ӱ��� (�������߷�ʼ�㹹��ƫ��)
            auto distance = glm::length(l.position - hitRec.hitPoint);
            int savedIndex = lastHitTriangleIndex;
            auto shadowRay = Ray{ hitRec.hitPoint + shadingNormal * 0.001f, out };
            auto shadowHit = closestHit(shadowRay);
            lastHitTriangleIndex = savedIndex; // �����Ӱ�ߵ�����

            // ��ɫ����
            auto c = shaderPrograms[hitRec.material.index()]->shade(-r.direction, out, shadingNormal);

            // δ���ڵ��򷵻ع��չ���
            if ((!shadowHit) || (shadowHit && shadowHit->t > distance)) {
                return c * l.intensity;
            }
            // ���ڵ����غ�ɫ
            else {
                return Vec3{ 0 };
            }
        }
        // δ���з��ر�������ɫ��
        else {
            return { 0, 0, 0 };
        }
    }

    // ��������ж������塢�����Ρ�ƽ�棩
    HitRecord PhongShadingRenderer::closestHit(const Ray& r) {
        HitRecord closestHitRec = nullopt;
        float closest = FLOAT_INF;

        // ����
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.01f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHitRec = hitRecord;
                lastHitTriangleIndex = -1;
            }
        }

        // ������
        for (int i = 0; i < static_cast<int>(scene.triangleBuffer.size()); ++i) {
            auto& t = scene.triangleBuffer[i];
            auto hitRecord = Intersection::xTriangle(r, t, 0.01f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHitRec = hitRecord;
                lastHitTriangleIndex = i;
            }
        }

        // ƽ��
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.01f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHitRec = hitRecord;
                lastHitTriangleIndex = -1;
            }
        }

        return closestHitRec;
    }
}
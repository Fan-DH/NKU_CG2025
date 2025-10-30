// Gouraud 着色渲染器实现
// 预计算顶点颜色，像素级采用重心坐标插值并进行阴影判断
#include "GouraudShadingRenderer.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"
#include <algorithm>
#include <cmath>

namespace GouraudShading
{
    using namespace std;

    // 释放渲染结果中的像素数组内存
    void GouraudShadingRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    // Gamma 校正：对 RGB 每个分量开平方
    RGB GouraudShadingRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    // 顶点位置量化键（减少浮点抖动带来的重复顶点判定问题）
    struct QKey { int x; int y; int z; };
    struct QKeyHash {
        size_t operator()(const QKey& k) const noexcept {
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

    // 计算三角形顶点法线：位置量化 + 相邻面法线累加 + 归一化
    void GouraudShadingRenderer::computeTriangleVertexNormals() {
        unordered_map<QKey, Vec3, QKeyHash, QKeyEq> accum;

        // 预先计算每个三角形的面法线
        vector<Vec3> faceNormals;
        faceNormals.reserve(scene.triangleBuffer.size());
        for (const auto& t : scene.triangleBuffer) {
            Vec3 e1 = t.v2 - t.v1;
            Vec3 e2 = t.v3 - t.v1;
            Vec3 n = glm::normalize(glm::cross(e1, e2));
            faceNormals.push_back(n);
        }

        // 将面法线累加到对应的顶点位置键上
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

        // 归一化得到每个唯一顶点位置的最终法线
        unordered_map<QKey, Vec3, QKeyHash, QKeyEq> vertexNormal;
        vertexNormal.reserve(accum.size());
        for (const auto& [k, v] : accum) {
            vertexNormal.emplace(k, glm::normalize(v));
        }

        // 为每个三角形提取三个顶点法线
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

    // 预计算每个三角形三个顶点的颜色（使用材质着色器 + 点光源）
    void GouraudShadingRenderer::precomputeTriangleVertexColors() {
        triVertexColors.clear();
        triVertexColors.resize(scene.triangleBuffer.size());

        // 若无点光源，顶点颜色置零
        if (scene.pointLightBuffer.empty()) {
            for (size_t i = 0; i < scene.triangleBuffer.size(); ++i) {
                triVertexColors[i] = { RGB{0}, RGB{0}, RGB{0} };
            }
            return;
        }

        const auto& l = scene.pointLightBuffer[0];
        const RGB ambientColor = (scene.ambient.type == NRenderer::Ambient::Type::CONSTANT)
            ? scene.ambient.constant : RGB{0};

        for (size_t i = 0; i < scene.triangleBuffer.size(); ++i) {
            const auto& tri = scene.triangleBuffer[i];
            const auto& nset = triVertexNormals[i];
            std::array<RGB, 3> colors;

            const Vec3 verts[3] = { tri.v1, tri.v2, tri.v3 };
            for (int j = 0; j < 3; ++j) {
                const Vec3& p = verts[j];
                const Vec3& n = glm::normalize(nset[j]);
                const Vec3 out = glm::normalize(l.position - p);
                const float ndotl = glm::dot(out, n);

                // 背面或入射角大于 90°，使用环境光
                if (ndotl <= 0.f) {
                    colors[j] = ambientColor;
                    continue;
                }

                // 视线方向（从顶点指向摄像机）
                const Vec3 in = glm::normalize(scene.camera.position - p);
                RGB c = shaderPrograms[tri.material.index()]->shade(in, out, n);

                // 顶点级阴影检测（避免自阴影）
                float distance = glm::length(l.position - p);
                Ray shadowRay{ p + n*0.001f, out };
                // 保存命中三角形索引以免污染主追踪
                int saved = lastHitTriangleIndex;
                auto shadowHit = closestHit(shadowRay);
                lastHitTriangleIndex = saved;

                if (!shadowHit || shadowHit->t > distance - 1e-4f) {
                    colors[j] = c;                // 不乘以强度，留到像素阶段
                } else {
                    colors[j] = ambientColor;     // 阴影用环境光
                }
            }
            triVertexColors[i] = colors;
        }
    }

    // 渲染主流程
    auto GouraudShadingRenderer::render() -> RenderResult {
        auto width = scene.renderOption.width;
        auto height = scene.renderOption.height;
        auto pixels = new RGBA[width * height];

        // 顶点从局部坐标转换到世界坐标
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // 计算三角形顶点法线
        computeTriangleVertexNormals();

        // 创建材质着色器
        ShaderCreator shaderCreator{};
        shaderPrograms.clear();
        shaderPrograms.reserve(scene.materials.size());
        for (auto& mtl : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(mtl, scene.textures));
        }

        // 预计算三角形顶点颜色
        precomputeTriangleVertexColors();

        // 像素级追踪
        for (int i = 0; i < static_cast<int>(height); ++i) {
            for (int j = 0; j < static_cast<int>(width); ++j) {
                // 发射相机射线
                auto ray = camera.shoot(float(j) / float(width), float(i) / float(height));
                // 追踪得到颜色
                auto color = trace(ray);
                // 色彩裁剪并进行 Gamma 校正
                color = clamp(color);
                color = gamma(color);
                pixels[(height - i - 1) * width + j] = { color, 1 };
            }
        }

        return { pixels, width, height };
    }

    // 计算重心坐标（返回 u,v,w），用于插值顶点颜色
    static inline std::array<float, 3> barycentric(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& p) {
        Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;
        if (fabs(denom) < 1e-8f) {
            // 退化情况：使用均匀权重
            return { 1.f/3.f, 1.f/3.f, 1.f/3.f };
        }
        float invDenom = 1.0f / denom;
        float v = (d11 * d20 - d01 * d21) * invDenom;
        float w = (d00 * d21 - d01 * d20) * invDenom;
        float u = 1.0f - v - w;
        return { u, v, w };
    }

    // 光线追踪：三角形使用 Gouraud 插值，其他几何使用点着色
    RGB GouraudShadingRenderer::trace(const Ray& r) {
        if (scene.pointLightBuffer.empty()) return { 0, 0, 0 };

        const auto& l = scene.pointLightBuffer[0];
        const RGB ambientColor = (scene.ambient.type == NRenderer::Ambient::Type::CONSTANT)
            ? scene.ambient.constant : RGB{0};

        lastHitTriangleIndex = -1;
        auto closestHitObj = closestHit(r);
        if (!closestHitObj) return { 0, 0, 0 };

        auto& hitRec = *closestHitObj;
        const Vec3 out = glm::normalize(l.position - hitRec.hitPoint);

        // 背面直接返回环境光
        if (glm::dot(out, hitRec.normal) <= 0.f) {
            return ambientColor;
        }

        // 阴影检测
        float distance = glm::length(l.position - hitRec.hitPoint);
        Ray shadowRay{ hitRec.hitPoint + hitRec.normal*0.001f, out };
        int saved = lastHitTriangleIndex;
        auto shadowHit = closestHit(shadowRay);
        lastHitTriangleIndex = saved;
        if (shadowHit && shadowHit->t <= distance - 1e-4f) {
            return ambientColor;
        }

        // 三角形：重心插值顶点颜色
        if (lastHitTriangleIndex >= 0 && lastHitTriangleIndex < static_cast<int>(scene.triangleBuffer.size())) {
            const auto& tri = scene.triangleBuffer[lastHitTriangleIndex];
            const auto& cols = triVertexColors[lastHitTriangleIndex];
            auto [u, v, w] = barycentric(tri.v1, tri.v2, tri.v3, hitRec.hitPoint);
            RGB c = cols[0] * u + cols[1] * v + cols[2] * w;
            return c * l.intensity;
        }
        // 非三角形：直接使用材质着色器
        else {
            RGB c = shaderPrograms[hitRec.material.index()]->shade(-r.direction, out, hitRec.normal);
            return c * l.intensity;
        }
    }

    // 最近命中检测：遍历球、三角形和面片
    HitRecord GouraudShadingRenderer::closestHit(const Ray& r) {
        HitRecord closestHitRec = nullopt;
        float closest = FLOAT_INF;

        // 球体
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.01f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHitRec = hitRecord;
                lastHitTriangleIndex = -1;
            }
        }

        // 三角形
        for (int i = 0; i < static_cast<int>(scene.triangleBuffer.size()); ++i) {
            auto& t = scene.triangleBuffer[i];
            auto hitRecord = Intersection::xTriangle(r, t, 0.01f, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHitRec = hitRecord;
                lastHitTriangleIndex = i;
            }
        }

        // 平面
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
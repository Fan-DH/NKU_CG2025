// ����Ͷ����Ⱦ��ʵ��
// ʵ���˻����Ĺ���Ͷ����Ⱦ�㷨��������Ӱ����
#include "RayCastRenderer.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

namespace RayCast
{
    // �ͷ���Ⱦ���
    // r: ��Ⱦ���
    void RayCastRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    // ٤��У��
    // rgb: �����RGB��ɫ
    // ����У�������ɫ
    RGB RayCastRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    // ��Ⱦ����
    // ������Ⱦ���
    auto RayCastRenderer::render() -> RenderResult {
        auto width = scene.renderOption.width;
        auto height = scene.renderOption.height;
        // �������ػ�����
        auto pixels = new RGBA[width*height];

        // ִ�ж���任
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // ������ɫ������
        ShaderCreator shaderCreator{};
        for (auto& mtl : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(mtl, scene.textures));
        }

        // ��ÿ�����ؽ��й���׷��
        for (int i=0; i<height; i++) {
            for (int j=0; j < width; j++) {
                // ���ɹ���
                auto ray = camera.shoot(float(j)/float(width), float(i)/float(height));
                // ׷�ٹ��߻�ȡ��ɫ
                auto color = trace(ray);
                // ��ɫ����
                color = clamp(color);
                color = gamma(color);
                pixels[(height-i-1)*width+j] = {color, 1};
            }
        }

        return {pixels, width, height};
    }
    
    // ����׷��
    // r: �������
    // ���ظù��߶�Ӧ����ɫ
    RGB RayCastRenderer::trace(const Ray& r) {
        // ���������û�е��Դ�����غ�ɫ
        if (scene.pointLightBuffer.size() < 1) return {0, 0, 0};
        // ��ȡ��һ�����Դ
        auto& l = scene.pointLightBuffer[0];
        // ����������ཻ����
        auto closestHitObj = closestHit(r);
        
        if (closestHitObj) {
            auto& hitRec = *closestHitObj;
            // �����Դ����
            auto out = glm::normalize(l.position - hitRec.hitPoint);
            // ����Դ�Ƿ��ڱ��汳��
            if (glm::dot(out, hitRec.normal) < 0) {
                return {0, 0, 0};
            }
            // ���㵽��Դ�ľ���
            auto distance = glm::length(l.position - hitRec.hitPoint);
            // ������Ӱ����
            auto shadowRay = Ray{hitRec.hitPoint, out};
            auto shadowHit = closestHit(shadowRay);
            // ������ɫ���
            auto c = shaderPrograms[hitRec.material.index()]->shade(-r.direction, out, hitRec.normal);
            // ���û���ڵ����ڵ����ڹ�Դ���棬������ɫ���
            if ((!shadowHit) || (shadowHit && shadowHit->t > distance)) {
                return c * l.intensity;
            }
            // ��������Ӱ�У����غ�ɫ
            else {
                return Vec3{0};
            }
        }
        else {
            return {0, 0, 0};
        }
    }

    // ����������ཻ����
    // r: �������
    // ����������ཻ��¼
    HitRecord RayCastRenderer::closestHit(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;
        // �����������ཻ
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        // ����������ε��ཻ
        for (auto& t : scene.triangleBuffer) {
            auto hitRecord = Intersection::xTriangle(r, t, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        // �����ƽ����ཻ
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.01, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        return closestHit; 
    }
}
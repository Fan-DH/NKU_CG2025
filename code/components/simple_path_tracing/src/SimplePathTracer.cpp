#include "server/Server.hpp"

#include "SimplePathTracer.hpp"

#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace SimplePathTracer
{
    /**
     * GammaУ������
     * ����ɫ����ƽ����У����ģ�����۶����ȵĸ�֪
     * @param rgb ԭʼ��ɫ
     * @return У�������ɫ
     */
    RGB SimplePathTracerRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    /**
     * ��Ⱦ�����������̣߳�
     * ����ָ����Χ�ڵ������У�����·��׷�ټ���
     * @param pixels ���ػ�����
     * @param width ͼ����
     * @param height ͼ��߶�
     * @param off ��ʼ��ƫ��
     * @param step �в��������ڶ��̷߳��䣩
     */
    void SimplePathTracerRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        for(int i=off; i<height; i+=step) {  // ������������
            for (int j=0; j<width; j++) {    // ����ÿ�е�����
                Vec3 color{0, 0, 0};         // ��ʼ��������ɫ
                
                // ���ز��������
                for (int k=0; k < samples; k++) {
                    // ���������������
                    auto r = defaultSamplerInstance<UniformInSquare>().sample2d();
                    float rx = r.x;
                    float ry = r.y;
                    float x = (float(j)+rx)/float(width);   // ��һ��x����
                    float y = (float(i)+ry)/float(height);  // ��һ��y����
                    
                    // ������������
                    auto ray = camera.shoot(x, y);
                    color += trace(ray, 0);  // ·��׷��
                }
                color /= samples;  // ƽ���������
                color = gamma(color);  // GammaУ��
                pixels[(height-i-1)*width+j] = {color, 1};  // �洢���أ���תy���꣩
            }
        }
    }

    /**
     * ����Ⱦ����
     * ��ʼ����ɫ����ִ�ж��߳���Ⱦ��������Ⱦ���
     * @return ��Ⱦ������������ݡ���ȡ��߶ȣ�
     */
    auto SimplePathTracerRenderer::render() -> RenderResult {
        // ��ʼ����ɫ������
        shaderPrograms.clear();
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(m, scene.textures));
        }

        // �������ػ�����
        RGBA* pixels = new RGBA[width*height]{};

        // ���ֲ�����ת������������
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);

        // ���߳���Ⱦ
        const auto taskNums = 8;  // ʹ��8���߳�
        thread t[taskNums];
        for (int i=0; i < taskNums; i++) {
            t[i] = thread(&SimplePathTracerRenderer::renderTask,
                this, pixels, width, height, i, taskNums);
        }
        for(int i=0; i < taskNums; i++) {
            t[i].join();  // �ȴ������߳����
        }
        getServer().logger.log("Done...");
        return {pixels, width, height};
    }

    /**
     * �ͷ���Ⱦ����ڴ�
     * @param r ��Ⱦ���
     */
    void SimplePathTracerRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;  // �ͷ����ػ�����
    }

    /**
     * ���ҹ��������������ཻ
     * �������м����壬�ҵ�������ཻ��
     * @param r ����
     * @return ������ཻ��¼
     */
    HitRecord SimplePathTracerRenderer::closestHitObject(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;
        
        // �������
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        
        // ���������
        for (auto& t : scene.triangleBuffer) {
            auto hitRecord = Intersection::xTriangle(r, t, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        
        // ���ƽ��
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.000001, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        return closestHit; 
    }
    
    /**
     * ���ҹ����������Դ���ཻ
     * �������������Դ���ҵ�������ཻ��
     * @param r ����
     * @return �ཻ����ͷ���ǿ��
     */
    tuple<float, Vec3> SimplePathTracerRenderer::closestHitLight(const Ray& r) {
        Vec3 v = {};
        HitRecord closest = getHitRecord(FLOAT_INF, {}, {}, {});
        
        // ��������Դ
        for (auto& a : scene.areaLightBuffer) {
            auto hitRecord = Intersection::xAreaLight(r, a, 0.000001, closest->t);
            if (hitRecord && closest->t > hitRecord->t) {
                closest = hitRecord;
                v = a.radiance;  // ��¼����ǿ��
            }
        }
        return { closest->t, v };
    }

    /**
     * ·��׷��������
     * ʵ�����ؿ���·��׷���㷨���ݹ���������ɫ
     * @param r ����
     * @param currDepth ��ǰ�ݹ����
     * @return ������ɫ
     */
    RGB SimplePathTracerRenderer::trace(const Ray& r, int currDepth) {
        // �ﵽ���ݹ���ȣ����ػ�����
        if (currDepth == depth) return scene.ambient.constant;
        
        // �������������͹�Դ�ཻ
        auto hitObject = closestHitObject(r);
        auto [ t, emitted ] = closestHitLight(r);
        
        // ������߻�������
        if (hitObject && hitObject->t < t) {
            auto mtlHandle = hitObject->material;
            // ʹ�ò�����ɫ������ɢ��
            auto scattered = shaderPrograms[mtlHandle.index()]->shade(r, hitObject->hitPoint, hitObject->normal);
            auto scatteredRay = scattered.ray;
            auto attenuation = scattered.attenuation;
            auto emitted = scattered.emitted;
            
            // �ݹ�׷��ɢ�����
            auto next = trace(scatteredRay, currDepth+1);
            float n_dot_in = glm::dot(hitObject->normal, scatteredRay.direction);
            float pdf = scattered.pdf;
            
            /**
             * ·��׷����Ⱦ����ʵ�֣�
             * emitted      - Le(p, w_0) �Է���
             * next         - Li(p, w_i) �����
             * n_dot_in     - cos<n, w_i> ������
             * attenuation  - BRDF ˫����ֲ�����
             * pdf          - p(w) �����ܶȺ���
             **/
            return emitted + attenuation * next * n_dot_in / pdf;
        }
        // ������߻��й�Դ
        else if (t != FLOAT_INF) {
            return emitted;
        }
        // ����δ�����κ�����
        else {
            return Vec3{0};
        }
    }
}
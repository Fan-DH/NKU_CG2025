#include "VertexTransformer.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace RayTracer
{
    /**
     * ִ�ж���任
     * �����������м�����Ķ���Ӿֲ�����ת������������
     * @param spScene ��������ָ��
     */
    void VertexTransformer::exec(SharedScene spScene) {
        auto& scene = *spScene;
        
        // �������г����ڵ�
        for (auto& node : scene.nodes) {
            Mat4x4 t{1};  // ��ʼ���任����Ϊ��λ����
            auto& model = spScene->models[node.model];
            
            // Ӧ��ƽ�Ʊ任
            t = glm::translate(t, model.translation);
            
            // ���ݼ���������Ӧ�ñ任
            if (node.type == Node::Type::TRIANGLE) {
                // �任�����ε���������
                for (int i=0; i<3; i++) {
                    auto& v = scene.triangleBuffer[node.entity].v[i];
                    v = t*Vec4{v, 1};  // �������任
                }
            }
            else if (node.type == Node::Type::SPHERE) {
                // �任����λ��
                auto& v = scene.sphereBuffer[node.entity].position;
                v = t*Vec4{v, 1};  // �������任
            }
            else if (node.type == Node::Type::PLANE) {
                // �任ƽ��λ��
                auto& v = scene.planeBuffer[node.entity].position;
                v = t*Vec4{v, 1};  // �������任
            }
        }
    }
}
// ��Ⱦ���������ʵ��
// �ṩ�˹���Ͷ����Ⱦ������Ⱦϵͳ�Ľӿ�����
#include "server/Server.hpp"
#include "component/RenderComponent.hpp"
#include "GouraudShadingRenderer.hpp"

using namespace std;
using namespace NRenderer;

namespace GouraudShading
{
    // ��������
    // �̳�����Ⱦ�����ʵ����Ⱦ�ӿ�
    class Adapter : public RenderComponent
    {
    public:
        // ��Ⱦ����
        // spScene: ����ָ��
        void render(SharedScene spScene) {
            // ��������Ͷ����Ⱦ��
            GouraudShadingRenderer gouraudShading{spScene};
            // ִ����Ⱦ
            auto result = gouraudShading.render();
            // ��ȡ��Ⱦ��������õ���Ļ
            auto [ pixels, width, height ] = result;
            getServer().screen.set(pixels, width, height);
            // �ͷ���Ⱦ���
            gouraudShading.release(result);         
        }
    };
}

// ��Ⱦ��������Ϣ
const static string description = 
    "Gouraud Shading Renderer.\n"
    "Supported: Lambertian, Phong\n"
    ;

// ע����Ⱦ��
REGISTER_RENDERER(GouraudShading, description, GouraudShading::Adapter);

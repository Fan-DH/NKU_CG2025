// ��Ⱦ���������ʵ��
// �ṩ�˹���Ͷ����Ⱦ������Ⱦϵͳ�Ľӿ�����
#include "server/Server.hpp"
#include "component/RenderComponent.hpp"
#include "PhongShadingRenderer.hpp"

using namespace std;
using namespace NRenderer;

namespace PhongShading
{
    // ��������
    // �̳�����Ⱦ�����ʵ����Ⱦ�ӿ�
    class Adapter : public RenderComponent
    {
    public:
        // ��Ⱦ����
        // spScene: ����ָ��
        void render(SharedScene spScene) {
            // ����Phong��ɫ��Ⱦ��
            PhongShadingRenderer phongShading{spScene};
            // ִ����Ⱦ
            auto result = phongShading.render();
            // ��ȡ��Ⱦ��������õ���Ļ
            auto [ pixels, width, height ] = result;
            getServer().screen.set(pixels, width, height);
            // �ͷ���Ⱦ���
            phongShading.release(result);
        }
    };
}

// ��Ⱦ��������Ϣ
const static string description = 
    "Phong Shading Renderer.\n"
    "Supported: Lambertian, Phong\n"
    ;

// ע����Ⱦ��
REGISTER_RENDERER(PhongShading, description, PhongShading::Adapter);

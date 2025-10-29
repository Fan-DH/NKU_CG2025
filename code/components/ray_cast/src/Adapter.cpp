// ��Ⱦ���������ʵ��
// �ṩ�˹���Ͷ����Ⱦ������Ⱦϵͳ�Ľӿ�����
#include "server/Server.hpp"
#include "component/RenderComponent.hpp"
#include "RayCastRenderer.hpp"

using namespace std;
using namespace NRenderer;

namespace RayCast
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
            RayCastRenderer rayCast{spScene};
            // ִ����Ⱦ
            auto result = rayCast.render();
            // ��ȡ��Ⱦ��������õ���Ļ
            auto [ pixels, width, height ] = result;
            getServer().screen.set(pixels, width, height);
            // �ͷ���Ⱦ���
            rayCast.release(result);
        }
    };
}

// ��Ⱦ��������Ϣ
const static string description = 
    "Ray Cast Renderer.\n"
    "Supported: Lambertian, Phong, Cook-Torrance\n"
    ;

// ע����Ⱦ��
REGISTER_RENDERER(RayCast, description, RayCast::Adapter);

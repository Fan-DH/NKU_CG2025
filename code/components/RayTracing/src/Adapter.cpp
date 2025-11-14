#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "component/RenderComponent.hpp"
#include "Camera.hpp"

#include "RayTracer.hpp"

using namespace std;
using namespace NRenderer;

namespace RayTracer
{
    /**
     * ��·��׷����Ⱦ��������
     * ʵ��RenderComponent�ӿڣ���Ϊ��Ⱦ����ϵͳ������
     */
    class Adapter : public RenderComponent
    {
        /**
         * ִ����Ⱦ
         * @param spScene ��������ָ��
         */
        void render(SharedScene spScene) {
            // ����·��׷����Ⱦ��
            RayTracerRenderer renderer{spScene};
            
            // ִ����Ⱦ
            auto renderResult = renderer.render();
            auto [ pixels, width, height ]  = renderResult;
            
            // ��������õ���Ļ
            getServer().screen.set(pixels, width, height);
            
            // �ͷ���Ⱦ����ڴ�
            renderer.release(renderResult);
        }
    };
}

// ��Ⱦ��������Ϣ
const static string description = 
    "A Ray Tracer. "
    "Only materials(Lambertian) are supported."
;

// ע����Ⱦ����ϵͳ
REGISTER_RENDERER(RayTracer, description, RayTracer::Adapter);

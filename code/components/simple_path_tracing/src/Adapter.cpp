#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "component/RenderComponent.hpp"
#include "Camera.hpp"

#include "SimplePathTracer.hpp"

using namespace std;
using namespace NRenderer;

namespace SimplePathTracer
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
            SimplePathTracerRenderer renderer{spScene};
            
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
    "A Simple Path Tracer. "
    "Only some simple primitives and materials(Lambertian) are supported."
    "\nPlease use scene file : cornel_area_light.scn";

// ע����Ⱦ����ϵͳ
REGISTER_RENDERER(SimplePathTracer, description, SimplePathTracer::Adapter);
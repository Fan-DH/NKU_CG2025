#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "component/RenderComponent.hpp"
#include "Camera.hpp"

#include "PhotonMapping.hpp"

using namespace std;
using namespace NRenderer;

namespace PhotonMapping
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
            PhotonMapperRenderer renderer{spScene};
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
    "Photon Mapping renderer. "
    "Builds a global photon map and renders indirect lighting via density estimation."
;

// ע����Ⱦ����ϵͳ
REGISTER_RENDERER(PhotonMapping, description, PhotonMapping::Adapter);

#include "ui/views/SceneView.hpp"
#include "asset/SceneBuilder.hpp"

// ������ͼʵ���ļ�
// ʵ���˳������ý���Ĺ���

namespace NRenderer
{
    // ���캯��
    // ��ʼ��������ͼ
    SceneView::SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View                  (position, size, uiContext, manager)  // ���û��๹�캯��
        , currComponentSelected (-1)                                  // ��ʼ����ǰѡ�����Ϊ-1
    {
    }

    // ��ʼ���Ƴ������ô���
    void SceneView::drawBeginWindow() {
        ImGui::Begin("Render Settings", nullptr, windowFlag);  // ������Ⱦ���ô���
    }

    // ���Ƴ������ý���
    void SceneView::draw() {
        cameraSetting();     // �������
        ImGui::Separator();  // �ָ���
        renderSetting();     // ��Ⱦ����
        ImGui::Separator();  // �ָ���
        ambientSetting();    // ����������
        ImGui::Separator();  // �ָ���
        componentSetting();  // �������
    }

    // ������ý���
    void SceneView::cameraSetting() {
        auto& camera = manager.renderSettingsManager.camera;
        ImGui::TextUnformatted("Camera:");
        float floatStep = 0.1;  // ��������������

        // �����������
        ImGui::InputFloat3("Position", &camera.position.x);        // ���λ��
        ImGui::InputFloat3("Up", &camera.up.x);                   // �Ϸ���
        ImGui::InputFloat3("LookAt", &camera.lookAt.x);           // �۲��
        ImGui::InputScalar("Fov", ImGuiDataType_Float, &camera.fov, &floatStep, NULL);                  // �ӳ���
        ImGui::InputScalar("Aspect", ImGuiDataType_Float, &camera.aspect, &floatStep, NULL);            // ���߱�
        ImGui::InputScalar("Aperture", ImGuiDataType_Float, &camera.aperture, &floatStep, NULL);        // ��Ȧ��С
        ImGui::InputScalar("FocusDistance", ImGuiDataType_Float, &camera.focusDistance, &floatStep, NULL);  // ����
    }

    // ��Ⱦ���ý���
    void SceneView::renderSetting() {
        int intStep = 1;  // ������������
        auto& rs = manager.renderSettingsManager.renderSettings;
        ImGui::TextUnformatted("Render Settings:");

        // ��Ⱦ��������
        ImGui::InputScalar("Width", ImGuiDataType_U32, &rs.width, &intStep, NULL, "%u");            // ��Ⱦ����
        ImGui::InputScalar("Height", ImGuiDataType_U32, &rs.height, &intStep, NULL, "%u");          // ��Ⱦ�߶�
        ImGui::InputScalar("Depth", ImGuiDataType_U32, &rs.depth, &intStep, NULL, "%u");            // ����׷�����
        ImGui::InputScalar("Sample Nums", ImGuiDataType_U32, &rs.samplesPerPixel, &intStep, NULL, "%u");  // ��������
    }

    // ���������ý���
    void SceneView::ambientSetting() {
        auto& as = manager.renderSettingsManager.ambientSettings;
        ImGui::TextUnformatted("Ambient:");

        // ����������ѡ��
        const string typeStr[2] = {"Constant", "Environment Map"};
        int curr = as.type == AmbientSettings::Type::CONSTANT ? 0 : 1;
        if(ImGui::BeginCombo("Type##AmbientSettings", typeStr[curr].c_str())) {
            for (int i=0; i<2; i++) {
                bool selected = curr == i;
                if (ImGui::Selectable((typeStr[i]+"##AmbientTypeItem").c_str(), &selected)) {
                    as.type = i == 0 ? AmbientSettings::Type::CONSTANT : AmbientSettings::Type::ENVIROMENT_MAP;
                    curr = i;
                }    
            }
            ImGui::EndCombo();
        }

        // ���ݻ�����������ʾ��ͬ����
        if (curr == 0) {
            // ��������������
            ImGui::ColorEdit3("Ambient", &as.ambient.r, ImGuiColorEditFlags_Float);
        }
        else if (curr == 1) {
            // ������ͼ����
            auto& v = as.mapTexture;
            if (ImGui::BeginCombo("Map Texture##AmbientEMAPTex", v.valid()? manager.assetManager.asset.textureItems[v.index()].name.c_str() : "")) {
                for (int i=0; i < manager.assetManager.asset.textureItems.size(); i++) {
                    bool selected = v.valid() ? (i == v.index()) : false;
                    if (ImGui::Selectable(manager.assetManager.asset.textureItems[i].name.c_str(), &selected)) {
                        v.setIndex(i);
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    // ��Ⱦ������ý���
    void SceneView::componentSetting() {
        ImGui::TextUnformatted("Render Component:");

        // ��ȡ���õ���Ⱦ����б�
        auto&& components = getServer().componentFactory.getComponentsInfo("Render");
        auto& componentManager = manager.componentManager;
        auto& state = uiContext.state;

        // ��Ⱦ���ѡ��������
        string comboPre = currComponentSelected != -1 && currComponentSelected < components.size() ?
            components[currComponentSelected].name : "";
        if (ImGui::BeginCombo("Render Component##SceneView", comboPre.c_str())) {
            for(int i=0; i<components.size(); i++) {
                bool selected = i == currComponentSelected;
                if (ImGui::Selectable((to_string(i+1) + ". " + components[i].name + "##SceneView").c_str(), &selected)) {
                    currComponentSelected = i;
                }
            }
            ImGui::EndCombo();
        }

        // ��ʾѡ�����������
        if (currComponentSelected != -1 && currComponentSelected < components.size()) {
            ImGui::TextWrapped(components[currComponentSelected].description.c_str());
            if (components[currComponentSelected].name == "RayTracer") {
                auto& rs = manager.renderSettingsManager.renderSettings;
                ImGui::Checkbox("KD-Tree", &rs.useKDTree);
            }
            if (components[currComponentSelected].name == "PhotonMapping") {
                ImGui::Checkbox("Visualize Photon Map", &manager.renderSettingsManager.renderSettings.visualizePhotonMap);
                int intStep = 1;
                ImGui::InputScalar("Photon Count", ImGuiDataType_U32, &manager.renderSettingsManager.renderSettings.photonCount, &intStep, NULL, "%u");
                ImGui::InputScalar("KNN N", ImGuiDataType_U32, &manager.renderSettingsManager.renderSettings.photonGatherK, &intStep, NULL, "%u");
            }
        }

        // ��Ⱦ��ť
        if (ImGui::Button("Render")) {
            if (currComponentSelected != -1 && currComponentSelected < components.size()) {
                // ����������ִ����Ⱦ
                auto& rs = manager.renderSettingsManager;
                SceneBuilder sceneBuilder{manager.assetManager.asset, rs.renderSettings, rs.ambientSettings, rs.camera};
                componentManager.exec<RenderComponent>(components[currComponentSelected], sceneBuilder.build());
            }
            else {
                getServer().logger.error("No render component is selected!");  // δѡ����Ⱦ���ʱ��ʾ����
            }
        }
    }

} // namespace NRenderer

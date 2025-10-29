#include "ui/views/ComponentProgressView.hpp"

// ���������ͼʵ���ļ�
// ʵ������Ⱦ���ִ�н��ȵ���ʾ����

namespace NRenderer
{
    // ��ʵ�ֵĴ��ڻ��ƺ���
    void ComponentProgressView::drawBeginWindow() {}
    void ComponentProgressView::drawEndWindow() {}
    void ComponentProgressView::drawSetup() {}
    void ComponentProgressView::drawFinish() {}

    // ���ý��ȴ��ڵ�λ�úʹ�С
    void ComponentProgressView::drawPosAndSize() {
        // ������Ļ����λ��
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        // ���ô���λ��Ϊ��Ļ����
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        // ���ô��ڳ�ʼ��С
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);
    }

    // ���ƽ��ȴ�������
    void ComponentProgressView::draw() {
        auto& componentManager = manager.componentManager;
        // ���������ڿ���״̬������ʾ���ȴ���
        if (componentManager.getState() == ComponentManager::State::IDLING) return;

        auto activeComponentInfo = componentManager.getActiveComponentInfo();  // ��ȡ��ǰ������Ϣ
        ImGui::OpenPopup("Executing Component");  // �򿪵�������

        // ��ʼ���Ƶ�������
        if (ImGui::BeginPopupModal("Executing Component", nullptr, 0)) {
            if (componentManager.getState() == ComponentManager::State::RUNNING) {
                // �����������
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("����ִ��: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::READY) {
                // ���׼������
                uiContext.state = UIContext::State::HOVER_COMPONENT_PROGRESS;
                ImGui::TextUnformatted(("׼��ִ��: " + activeComponentInfo.id).c_str());
            }
            else if (componentManager.getState() == ComponentManager::State::FINISH) {
                // ���ִ�����
                auto execTime = componentManager.getLastExecTime();  // ��ȡִ��ʱ��
                componentManager.finish();  // ������ִ��

                // ���ɲ���ʾ�����Ϣ
                string logInfo{};
                logInfo = activeComponentInfo.id + "ִ�����. Time: " + to_string(execTime.count()) + "s";
                getServer().logger.success(logInfo);

                uiContext.state = UIContext::State::NORMAL;  // �ָ�����״̬
                ImGui::CloseCurrentPopup();  // �رյ�������
            }
            ImGui::EndPopup();
        }
    }
}
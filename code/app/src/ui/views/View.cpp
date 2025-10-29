#include "ui/views/View.hpp"

// ��ͼ����ʵ���ļ�
// ʵ��������UI��ͼ�Ļ�������

namespace NRenderer
{
    // ��ʾ��ͼ
    // ������ͼ����ʾ״̬�����û��ƺ���
    void View::display() {
        drawSetup();  // ����ǰ������

        // �������״̬
        if (disable) {
            windowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus;  // ��ֹ����ǰ��
            windowFlag |= ImGuiWindowFlags_NoInputs;               // ��ֹ����
            windowFlag |= ImGuiWindowFlags_NoFocusOnAppearing;     // ��ֹ��ȡ����
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.7f);  // ���ð�͸��
        }

        // �����С����
        if (!resizable) {
            windowFlag |= ImGuiWindowFlags_NoResize;  // ��ֹ������С
        }

        // ���ƿɼ�����ͼ
        if (this->visible) {
            this->drawPosAndSize();     // ����λ�úʹ�С
            this->drawBeginWindow();    // ��ʼ���ƴ���
            this->draw();               // ��������
            this->drawEndWindow();      // �������ƴ���
        }

        drawFinish();  // ���ƺ������
    }

    // ����ǰ������
    void View::drawSetup() {
    }

    // �������ƴ���
    void View::drawEndWindow() {
        ImGui::End();
    }

    // ������ͼ��λ�úʹ�С
    void View::drawPosAndSize() {
        ImGui::SetNextWindowPos({position.x, position.y}, ImGuiCond_FirstUseEver);    // ���ó�ʼλ��
        ImGui::SetNextWindowSize({size.x, size.y}, ImGuiCond_FirstUseEver);          // ���ó�ʼ��С
    }

    // ���ƺ��������
    void View::drawFinish() {
        windowFlag = 0;  // ���ô��ڱ�־
        if (disable) {
            ImGui::PopStyleVar();  // �ָ���ʽ
        }
    }

    // ����������ʾ
    // desc: �����ı�
    void View::makeHelper(const char* desc)
    {
        ImGui::TextDisabled("(?)");  // ��ʾ����ͼ��
        if (ImGui::IsItemHovered())  // �������ͣʱ
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);  // �����ı�����
            ImGui::TextUnformatted(desc);                          // ��ʾ�����ı�
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // ���캯��
    // ��ʼ����ͼ�Ļ�������
    View::View(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : position              (position)     // ��ͼλ��
        , size                  (size)         // ��ͼ��С
        , uiContext             (uiContext)    // UI������
        , manager               (manager)      // ������
        , disable               (false)        // Ĭ������
        , visible               (true)         // Ĭ�Ͽɼ�
        , resizable             (true)         // Ĭ�Ͽɵ�����С
    {}
} // namespace NRenderer

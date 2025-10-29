#include "ui/views/LogView.hpp"

// ��־��ͼʵ���ļ�
// ʵ����ϵͳ��־����ʾ����

#include "server/Server.hpp"

#undef ERROR

namespace NRenderer
{
    // ��ʼ������־����
    void LogView::drawBeginWindow() {
        ImGui::Begin("Log", nullptr, windowFlag);  // ��������Ϊ"Log"�Ĵ���
    }

    // ������־����
    void LogView::draw() {
        // ��������ť
        bool clear = ImGui::Button("Clear");
        if (clear) {
            getServer().logger.clear();  // ���������־
        }
            
        // ������־���ݹ�������
        ImGui::BeginChild("Log Content", {}, true);
        {
            // ���岻ͬ������־����ɫ
            const ImVec4 green{ 0, 1, 0, 1 };   // �ɹ���ϢΪ��ɫ
            const ImVec4 red{1, 0, 0, 1};       // ������ϢΪ��ɫ
            const ImVec4 yellow{1, 1, 0, 1};    // ������ϢΪ��ɫ

            // ��ȡ����ʾ������־��Ϣ
            auto msgs = getServer().logger.get();
            for (int i=0; i<msgs.nums; i++) {
                auto& msg = msgs.msgs[i];
                switch (msg.type)
                {
                case Logger::LogType::SUCCESS:  // �ɹ���Ϣ
                    ImGui::PushStyleColor(ImGuiCol_Text, green);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::WARNING:  // ������Ϣ
                    ImGui::PushStyleColor(ImGuiCol_Text, yellow);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                case Logger::LogType::ERROR:    // ������Ϣ
                    ImGui::PushStyleColor(ImGuiCol_Text, red);
                    ImGui::TextWrapped(msg.message.c_str());
                    ImGui::PopStyleColor();
                    break;
                default:                        // ��ͨ��Ϣ
                    ImGui::TextUnformatted(msg.message.c_str());
                    break;
                }
            } 
        }
        ImGui::EndChild();  // ������������
    }
    
} // namespace NRenderer

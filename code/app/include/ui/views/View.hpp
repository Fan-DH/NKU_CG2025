#pragma once
#ifndef __NR_VIEW_HPP__
#define __NR_VIEW_HPP__

// ��ͼ����ͷ�ļ�
// ����������UI��ͼ�Ļ����ӿں͹���

#include "ui/UIContext.hpp"
#include "manager/Manager.hpp"
#include "geometry/vec.hpp"
#include "imgui.h"

namespace NRenderer
{
    // ��ͼ����
    // Ϊ����UI��ͼ�ṩ�������ܺͽӿ�
    class View
    {
    protected:
        Vec2 position;           // ��ͼλ��
        Vec2 size;              // ��ͼ��С
        UIContext& uiContext;    // UI����������
        Manager& manager;        // ����������

        bool disable;           // �Ƿ����
        bool visible;           // �Ƿ�ɼ�
        bool resizable;         // �Ƿ�ɵ�����С

        ImGuiWindowFlags windowFlag = 0;  // ImGui���ڱ�־

        // ����ǰ������
        virtual void drawSetup();
        // ���ƺ������
        virtual void drawFinish();
        // ����λ�úʹ�С
        virtual void drawPosAndSize();
        // ��ʼ���ƴ��ڣ����麯����
        virtual void drawBeginWindow() = 0;
        // �������ƴ���
        virtual void drawEndWindow();
        // �������ݣ����麯����
        virtual void draw() = 0;

        // ����������ʾ
        // desc: �����ı�
        void makeHelper(const char* desc);
    public:
        // ���캯��
        // position: ��ͼλ��
        // size: ��ͼ��С
        // uiContext: UI������
        // manager: ������
        View(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
        // ���ÿ������캯��
        View(const View&) = delete;
        
        // ��ʾ��ͼ
        void display();
        
        // ������ͼ�ɼ���
        // flag: �Ƿ�ɼ�
        inline
        void setVisibility(bool flag) {
            visible = flag;
        }

        // ������ͼ�Ƿ����
        // flag: �Ƿ����
        inline
        void setDisable(bool flag) {
            disable = flag;
        }

        // ������ͼ�Ƿ�ɵ�����С
        // flag: �Ƿ�ɵ�����С
        inline
        void setResizable(bool flag) {
            resizable = flag;
        }
    };
} // namespace NRenderer


#endif
#pragma once
#ifndef __NR_SCENE_VIEW_HPP__
#define __NR_SCENE_VIEW_HPP__

// ������ͼͷ�ļ�
// �����˳������ú����õĽ���

#include "View.hpp"

namespace NRenderer
{
    // ������ͼ��
    // ������ʾ�ͱ༭�������������Ⱦ����������������
    class SceneView: public View
    {
    private:
        // ��д�����麯��
        virtual void drawBeginWindow() override;  // ��ʼ���ƴ���
        virtual void draw() override;            // ��������

        void cameraSetting();                    // ������ý���
        void renderSetting();                    // ��Ⱦ���ý���
        void ambientSetting();                   // ���������ý���
        void componentSetting();                 // ������ý���

        int currComponentSelected;               // ��ǰѡ�е��������
    public:
        // ���캯��
        // position: ��ͼλ��
        // size: ��ͼ��С
        // uiContext: UI������
        // manager: ������
        SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif
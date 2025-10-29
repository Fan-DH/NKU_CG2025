#pragma once
#ifndef __NR_SCREEN_VIEW_HPP__
#define __NR_SCREEN_VIEW_HPP__

// ��Ļ��ͼͷ�ļ�
// ��������Ⱦ�����Ԥ������ʾ��ͼ

#include "View.hpp"
#include "utilities/GlImage.hpp"
#include "utilities/GlShader.hpp"

namespace NRenderer
{
    // ��Ļ��ͼ��
    // ������ʾ��Ⱦ����ͳ���Ԥ��
    class ScreenView: public View
    {
    private:
        // ��д�����麯��
        virtual void drawBeginWindow() override;  // ��ʼ���ƴ���
        virtual void draw() override;            // ��������

        void drawGlPreview();                    // ����OpenGLԤ��
        void preview();                          // Ԥ��ģʽ
        void result();                           // ���ģʽ
        float getShrinkNum();                    // ��ȡ���ű���

        GlShader nodeShader;                     // �ڵ���ɫ��
        GlShader lightShader;                    // ��Դ��ɫ��

        GlId glFBO;                              // ֡�������
        GlId glRBO;                              // ��Ⱦ�������

        unsigned int prePreviewHeight;            // Ԥ��ǰ�߶�
        unsigned int prePreviewWidth;             // Ԥ��ǰ���

        unsigned int previewHeight;               // Ԥ���߶�
        unsigned int previewWidth;                // Ԥ�����

        // ����ϵ����ö��
        enum class CoordinateType {
            LEFT_HANDED,                          // ��������ϵ
            RIGHT_HANDED                          // ��������ϵ
        };
        CoordinateType previewCoordinateType;     // Ԥ������ϵ����

        bool isPreviewSizeChange();               // ���Ԥ���ߴ��Ƿ�ı�
        void updatePreviewSize(unsigned int x, unsigned int y);  // ����Ԥ���ߴ�
        void updatePrePreviewSize();              // ����Ԥ��ǰ�ߴ�

        void genFB();                             // ����֡����
        void previewNode(const NodeItem& n);      // Ԥ���ڵ�
        void previewLight(const LightItem& l);    // Ԥ����Դ

        void align(const Vec2& size);             // ������ͼ��С

        // ��ͼ����ö��
        enum class ViewType
        {
            PREVIEW,                              // Ԥ��ģʽ
            RESULT                                // ���ģʽ
        };

        ViewType viewType;                        // ��ǰ��ͼ����

        int shrinkLevel;                          // ���ż���

        GlImageId renderResult;                   // ��Ⱦ�������
        GlImageId previewResult;                  // Ԥ���������
    public:
        // ���캯��
        // position: ��ͼλ��
        // size: ��ͼ��С
        // uiContext: UI������
        // manager: ������
        ScreenView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif
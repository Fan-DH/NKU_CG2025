#pragma once
#ifndef __NR_COMPONENT_PROGRESS_VIEW_HPP__
#define __NR_COMPONENT_PROGRESS_VIEW_HPP__

// ���������ͼͷ�ļ�
// ��������ʾ��Ⱦ������ȵĽ���

#include "View.hpp"

namespace NRenderer
{
   // ���������ͼ��
   // ������ʾ��Ⱦ�����ִ�н���
   class ComponentProgressView: public View
   {
    private:
        // ��д�����麯��
        virtual void drawSetup();         // ����ǰ������
        virtual void drawFinish();        // ���ƺ������
        virtual void drawPosAndSize();    // ����λ�úʹ�С
        virtual void drawBeginWindow();   // ��ʼ���ƴ���
        virtual void drawEndWindow();     // �������ƴ���
        virtual void draw();              // ��������
    public:
        using View::View;                 // ʹ�û���Ĺ��캯��
   };
}

#endif
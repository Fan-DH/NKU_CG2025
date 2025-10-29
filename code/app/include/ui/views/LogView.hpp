#pragma once
#ifndef __NR_LOG_VIEW_HPP__
#define __NR_LOG_VIEW_HPP__

// ��־��ͼͷ�ļ�
// ��������ʾϵͳ��־�Ľ���

#include "View.hpp"

namespace NRenderer
{
    // ��־��ͼ��
    // ������ʾϵͳ����ʱ����־��Ϣ
    class LogView : public View
    {
    protected:
        // ��д�����麯��
        virtual void draw() override;             // ������־����
        virtual void drawBeginWindow() override;  // ��ʼ���ƴ���
    public:
        using View::View;                         // ʹ�û���Ĺ��캯��
    };
} // namespace NRenderer


#endif
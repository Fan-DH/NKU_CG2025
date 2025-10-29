// ��Ļ�ඨ��
// ������Ⱦ��������ػ��������ṩ�̰߳�ȫ�ķ��ʻ���
#pragma once
#ifndef __NR_SCREEN_HPP__
#define __NR_SCREEN_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include <mutex>

namespace NRenderer
{
    // ��Ļ��
    // ������Ⱦ������������ݣ�֧���̰߳�ȫ�ĸ��ºͷ���
    class DLL_EXPORT Screen
    {
    private:
        RGBA* pixels;           // �������ݻ�����
        unsigned int width;     // ��Ļ���
        unsigned int height;    // ��Ļ�߶�
        mutable bool updated;   // ���±�־
        mutable mutex mtx;      // ����������֤�̰߳�ȫ

    public:
        // Ĭ�Ϲ��캯��
        Screen();
        // ���ÿ������캯��
        Screen(const Screen&) = delete;
        // �����������ͷ����ػ�����
        ~Screen();

        // ������Ļ����
        // �������ػ����������ݺͳߴ�
        void set(RGBA* pixels, int width, int height);

        // ��ȡ��Ļ���
        unsigned int getWidth() const;
        // ��ȡ��Ļ�߶�
        unsigned int getHeight() const;
        // ��ȡ��������
        const RGBA* getPixels() const;
        // �ͷ����ػ�����
        void release();
        // ����Ƿ��и���
        bool isUpdated() const;
    };  
} // namespace NRenderer


#endif
#pragma once
#ifndef __NR_UI_HPP__
#define __NR_UI_HPP__

// �û�����ͷ�ļ�
// ��������Ⱦ������������

#include "glad/glad.h"
#include "glfw3.h"
#include "glfw3native.h"

#include "UIContext.hpp"
#include "manager/Manager.hpp"

#include <string>
#include <vector>

#include "views/View.hpp"

namespace NRenderer
{
    using namespace std;

    // �û�������
    // ����������Ⱦ�����ڼ���������ͼ
    class UI
    {
    private:
        unsigned int height;      // ���ڸ߶�
        unsigned int width;       // ���ڿ��
        string title;            // ���ڱ���
        GLFWwindow* window;      // GLFW���ھ��

        Manager manager;         // ������ʵ��
        UIContext uiContext;     // UI������ʵ��

        vector<View*> views;     // ��ͼ�б�
    public:
        // Ĭ�Ϲ��캯��
        UI();
        // ���������캯��
        // width: ���ڿ��
        // height: ���ڸ߶�
        // title: ���ڱ���
        UI(unsigned int width, unsigned int height, const string& title);
        // ���ÿ������캯��
        UI(const UI&) = delete;
        // ��������
        ~UI();

        // ��ʼ��UI
        void init();
        // ����UI��ѭ��
        void run();
    };
} // namespace NRenderer


#endif
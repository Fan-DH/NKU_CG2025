// ��Ⱦ������ඨ��
// ������Ⱦ������Ļ��࣬�ṩ��Ⱦ���ܵĽӿ�
#pragma once
#ifndef __NR_RENDER_COMPONENT_HPP__
#define __NR_RENDER_COMPONENT_HPP__

#include "Component.hpp"
#include "scene/Scene.hpp"

#include <functional>

namespace NRenderer
{
    // ��Ⱦ�������
    // ��������Ⱦ���Ļ����ӿڣ�������Ⱦ������ִ������
    class DLL_EXPORT RenderComponent: public Component
    {
    private:
        // ���麯�����������Ⱦʵ��
        // �������ʵ�ִ˷���������������Ⱦ�߼�
        virtual void render(SharedScene spScene) = 0;
    public:
        // ִ����Ⱦ����
        // onStart: ��Ⱦ��ʼʱ�Ļص�
        // onFinish: ��Ⱦ����ʱ�Ļص�
        // spScene: Ҫ��Ⱦ�ĳ���
        void exec(function<void()> onStart, function<void()> onFinish, SharedScene spScene);
    };
}

// ��Ⱦ��ע���
// ����ע���µ���Ⱦ�����
#define REGISTER_RENDERER(__NAME__, __DESCRIPTION__, __CLASS__)          REGISTER_COMPONENT(Render, __NAME__, __DESCRIPTION__, __CLASS__)

// ��������Ⱦ��ע���
// �򻯰����Ⱦ��ע��꣬����Ҫ�ṩ������Ϣ
#define REGISTER_RENDERER_NO_DESCRIPTION(__NAME__, __CLASS__)            REGISTER_RENDERER(__NAME__, "" ,__CLASS__)
#endif
#pragma once
#ifndef __NR_MANAGER_HPP__
#define __NR_MANAGER_HPP__

// �������ܿ�ͷ�ļ�
// ������������Ⱦϵͳ������������

#include "AssetManager.hpp"
#include "RenderSettingsManager.hpp"
#include "ComponentManager.hpp"

namespace NRenderer
{
    // ����������
    // ����Э���ʲ�����Ⱦ���ú�����Ĺ���
    class Manager
    {
    public:
        AssetManager assetManager;              // �ʲ�������ʵ��
        RenderSettingsManager renderSettingsManager;  // ��Ⱦ���ù�����ʵ��
        ComponentManager componentManager;       // ���������ʵ��

        // ���캯��
        Manager();
        // Ĭ����������
        ~Manager() = default;
    };
} // namespace NRenderer


#endif
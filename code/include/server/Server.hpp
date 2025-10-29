// �������ඨ��
// �ṩȫ�ַ������Դ����������־����Ļ�����ϵͳ
#pragma once
#ifndef __NR_SERVER_HPP__
#define __NR_SERVERHPP__

#include "Screen.hpp"
#include "Logger.hpp"
#include "component/ComponentFactory.hpp"

namespace NRenderer
{
    // �������ṹ��
    // ���й�����Ⱦϵͳ�ĺ��ķ���
    struct DLL_EXPORT Server
    {
        Logger logger = {};             // ��־ϵͳ
        Screen screen = {};             // ��Ļ����
        ComponentFactory componentFactory = {};  // �������
        Server() = default;
    };
} // namespace NRenderer

// ȫ�ַ��������ʺ���
// �ṩ�Է�����ʵ����ȫ�ַ��ʵ�
extern "C" {
    DLL_EXPORT
    NRenderer::Server& getServer();
}

#endif
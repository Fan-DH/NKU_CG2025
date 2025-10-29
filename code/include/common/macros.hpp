// �����궨���ͨ�ù���
// ������ƽ̨DLL���뵼���궨�������ָ����صĸ�����
#pragma once
#ifndef __NR_MACROS_HPP__
#define __NR_MACROS_HPP__

// Windowsƽ̨�µ�DLL���뵼������
#ifdef _WIN32
    #define DLL_IMPORT __declspec(dllimport)
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_IMPORT
    #define DLL_EXPORT
#endif

#include <memory>

// �������͵�����ָ�����
// ʹ��ʾ����SHARE(MyClass) ������ SharedMyClass ���ͣ��ȼ��� std::shared_ptr<MyClass>
#define SHARE(__T__) using Shared##__T__ = std::shared_ptr<__T__>

#endif
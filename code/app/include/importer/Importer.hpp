#pragma once
#ifndef __NR_IMPORTER_HPP__
#define __NR_IMPORTER_HPP__

// ����������ͷ�ļ�
// �������ʲ��������Ļ����ӿ�

#include <string>
#include "asset/Asset.hpp"

namespace NRenderer
{
    using namespace std;

    // ����������
    // Ϊ��ͬ���͵��ʲ��������ṩͳһ�Ľӿ�
    class Importer
    {
    protected:
        string lastErrorInfo;   // ���һ�δ�����Ϣ
    public:
        // �����ʲ�
        // asset: Ŀ���ʲ�����
        // path: �ʲ��ļ�·��
        // ����: �����Ƿ�ɹ�
        virtual bool import(Asset& asset, const string& path) = 0;

        // ��ȡ������Ϣ
        // ����: ���һ�δ�����Ϣ
        inline
        string getErrorInfo() const {
            return lastErrorInfo;
        }

        // ���캯��
        Importer()
            : lastErrorInfo         ()
        {}

        // ����������
        virtual ~Importer() = default;
    };
    SHARE(Importer);
}

#endif
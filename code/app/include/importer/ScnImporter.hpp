#pragma once
#ifndef __NR_SCN_IMPORTER_HPP__
#define __NR_SCN_IMPORTER_HPP__

// SCN�����ļ�������ͷ�ļ�
// �����˵���SCN��ʽ�����ļ��Ĺ���

#include "Importer.hpp"
#include <map>

namespace NRenderer
{
    using namespace std;

    // SCN�����ļ���������
    // ��������͵���SCN��ʽ�ĳ����ļ�
    class ScnImporter: public Importer
    {
    private:
        // �������ʲ���
        // asset: Ŀ���ʲ�����
        // file: �ļ���
        // mtlMap: �������Ƶ�������ӳ��
        // ����: �����Ƿ�ɹ�
        bool parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);

        // ����ģ�Ͳ���
        // asset: Ŀ���ʲ�����
        // file: �ļ���
        // mtlMap: �������Ƶ�������ӳ��
        // ����: �����Ƿ�ɹ�
        bool parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);

        // ������Դ����
        // asset: Ŀ���ʲ�����
        // file: �ļ���
        // ����: �����Ƿ�ɹ�
        bool parseLgt(Asset& asset, ifstream& file);
    public:
        // ����SCN�ļ�
        // asset: Ŀ���ʲ�����
        // path: SCN�ļ�·��
        // ����: �����Ƿ�ɹ�
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif
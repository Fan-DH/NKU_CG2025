#pragma once
#ifndef __OBJ_IMPORTER_HPP__
#define __OBJ_IMPORTER_HPP__

// OBJ�ļ�������ͷ�ļ�
// �����˵���OBJ��ʽ3Dģ���ļ��Ĺ���

#include "Importer.hpp"
#include <map>

#include <unordered_map>

namespace NRenderer
{
    using namespace std;

    // OBJ�ļ���������
    // ��������͵���OBJ��ʽ��3Dģ���ļ�
    class ObjImporter: public Importer
    {
    private:
        // ����MTL�����ļ�
        // asset: Ŀ���ʲ�����
        // path: MTL�ļ�·��
        // file: MTL�ļ���
        // mtlMap: �������Ƶ�������ӳ��
        // ����: �����Ƿ�ɹ�
        bool parseMtl(Asset& asset, const string& path, ifstream& file, unordered_map<string, size_t>& mtlMap);
    public:
        // ����OBJ�ļ�
        // asset: Ŀ���ʲ�����
        // path: OBJ�ļ�·��
        // ����: �����Ƿ�ɹ�
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif
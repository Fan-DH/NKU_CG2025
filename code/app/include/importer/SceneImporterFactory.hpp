#pragma once
#ifndef __NR_SCENE_IMPORTER_FACTORY_HPP__
#define __NR_SCENE_IMPORTER_FACTORY_HPP__

// ��������������ͷ�ļ�
// �����˴����͹���ͬ���ͳ����������Ĺ�����

#include <string>
#include <unordered_map>

#include "Importer.hpp"
#include "ScnImporter.hpp"
#include "ObjImporter.hpp"

namespace NRenderer
{
    using namespace std;

    // ����������������
    // ���𴴽��͹���ͬ�ļ���ʽ�ĳ���������
    class SceneImporterFactory
    {
    private:
        unordered_map<string, SharedImporter> importerMap;  // �ļ���չ������������ӳ��
    public:
        // ��ȡ��������
        // ����: ����ʵ��������
        static SceneImporterFactory& instance() {
            static SceneImporterFactory f;
            return f;
        }

        // ���캯��
        // ��ʼ��֧�ֵĵ���������
        SceneImporterFactory() {
            importerMap["scn"] = make_shared<ScnImporter>();  // ���SCN��ʽ������
            importerMap["obj"] = make_shared<ObjImporter>();  // ���OBJ��ʽ������
        }

        // ��ȡָ���ļ���ʽ�ĵ�����
        // ext: �ļ���չ��
        // ����: ��Ӧ�ĵ�����ָ�룬�����֧�ָø�ʽ�򷵻�nullptr
        SharedImporter importer(const string& ext) {
            auto it = importerMap.find(ext);
            if (it!=importerMap.end()) {
                return it->second;
            }
            return nullptr;
        }
    };
}

#endif
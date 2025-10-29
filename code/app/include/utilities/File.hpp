#pragma once
#ifndef __NR_FILE_HPP__
#define __NR_FILE_HPP__

// �ļ�������ͷ�ļ�
// �ṩ���ļ�·������Ļ�������

#include "scene/Model.hpp"
#include "scene/Light.hpp"

namespace NRenderer
{
    using namespace std;

    // �ļ�������
    // �ṩ�˻�ȡ�ļ���չ�����ļ����ľ�̬����
    struct File
    {
        // ��ȡ�ļ���չ��
        // filePath: �ļ�·��
        // �����ļ�����չ������������ţ�
        static string getFileExtension(const string& filePath) {
			auto pos = filePath.find_last_of('.');
			return filePath.substr(pos+1, filePath.size()-pos-1);
		}

        // ��ȡ�ļ���
        // filePath: �ļ�·��
        // �����ļ�����������չ����������·����
        static string getFileName(const string& filePath) {
            auto pos = filePath.find_last_of("\\/");
            string fileName;
            if(pos != string::npos)
                fileName = filePath.substr(pos + 1, filePath.size() - pos - 1);
            else
                fileName = filePath;
            return fileName;
        }
    };

    // �ļ�����ָ�����Ͷ���
    using SharedFile = shared_ptr<File>;
} // namespace NRenderer

#endif
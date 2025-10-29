#pragma once
#ifndef __NR_TEXTURE_IMPORTER_HPP__
#define __NR_TEXTURE_IMPORTER_HPP__

// ��������ͷ�ļ�
// �����˵��������ļ��Ĺ���

#include "Importer.hpp"

namespace NRenderer
{
    // ����������
    // ������ͽ��������ļ�
    class TextureImporter : public Importer
    {
    public:
        // Ĭ�Ϲ��캯��
        TextureImporter() = default;
        // Ĭ����������
        ~TextureImporter() = default;

        // ���������ļ�
        // asset: Ŀ���ʲ�����
        // path: �����ļ�·��
        // ����: �����Ƿ�ɹ�
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif
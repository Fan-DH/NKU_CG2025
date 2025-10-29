#pragma once
#ifndef __NR_ITEM_HPP__
#define __NR_ITEM_HPP__

// �ʲ������ͷ�ļ�
// �����˳����������ʲ���Ļ����ṹ

#include <string>

namespace NRenderer
{
    using namespace std;
    // OpenGL����ID����
    using GlId = unsigned int;

    // �ʲ������
    // ���г����ʲ���ģ�͡����ʡ���Դ�ȣ��Ļ���
    struct Item
    {
        string name;    // �ʲ�������

        // Ĭ�Ϲ��캯��
        // ����һ��δ�������ʲ���
        Item()
            : name      ("undefined")
        {}
    };

    // OpenGL�������ݽṹ
    // �洢����OpenGL���ƵĶ�������
    struct GlDrawData
    {
        vector<Vec3> positions;  // ����λ������
    };
    SHARE(GlDrawData);  // ��������ָ������
}

#endif
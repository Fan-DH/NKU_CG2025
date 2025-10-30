#pragma once
#ifndef __VERTEX_TRANSFORM_HPP__
#define __VERTEX_TRANSFORM_HPP__

// ����任��ͷ�ļ�
// �����˳�����ģ�Ͷ��������任����

#include "scene/Scene.hpp"

namespace PhongShading
{
    using namespace NRenderer;

    // ����任����
    // ���𽫳����е�ģ�ʹӾֲ�����ת������������
    class VertexTransformer
    {
    private:
    public:
        // ִ�ж���任
        // spScene: ����ָ��
        // ������������ģ�ʹӾֲ�����ת������������
        void exec(SharedScene spScene);
    };
}

#endif
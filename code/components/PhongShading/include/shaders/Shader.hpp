#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

// ��ɫ������ͷ�ļ�
// ������������ɫ���Ļ����ӿ�

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include "scene/Scene.hpp"

namespace PhongShading
{
    using namespace NRenderer;
    using namespace std;

    constexpr float PI = 3.1415926535898f;  // Բ���ʳ���

    // ��ɫ������
    // �ṩ�˲�����ɫ����Ļ����ӿ�
    class Shader
    {
    protected:
        Material& material;          // ���ʲ���
        vector<Texture>& textureBuffer;  // ��������

    public:
        // ���캯��
        // material: ���ʲ���
        // textures: ��������
        Shader(Material& material, vector<Texture>& textures)
            : material              (material)
            , textureBuffer         (textures)
        {}

        // ������ɫ���
        // in: ���䷽��
        // out: ���䷽��
        // normal: ���淨��
        // ������ɫ�������ɫֵ
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const = 0;
    };
    SHARE(Shader);  // ��������ָ������
}

#endif
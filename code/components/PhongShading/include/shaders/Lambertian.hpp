#pragma once
#ifndef __LAMBERTIAN_HPP__
#define __LAMBERTIAN_HPP__

// Lambert������ɫ��ͷ�ļ�
// ������Lambert��������ʵ���ɫ����

#include "Shader.hpp"

namespace PhongShading
{
    // Lambert������ɫ����
    // ʵ����Lambert������ģ��
    class Lambertian : public Shader
    {
    private:
        Vec3 diffuseColor;  // ��������ɫ

    public:
        // ���캯��
        // material: ���ʲ���
        // textures: ��������
        Lambertian(Material& material, vector<Texture>& textures);

        // ������ɫ���
        // in: ���䷽��
        // out: ���䷽��
        // normal: ���淨��
        // ������ɫ�������ɫֵ
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const;
    };
}

#endif
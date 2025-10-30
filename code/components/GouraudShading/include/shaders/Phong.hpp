#pragma once
#ifndef __PHONG_HPP__
#define __PHONG_HPP__

// Phong������ɫ��ͷ�ļ�
// ������Phong����ģ�͵���ɫ����

#include "Shader.hpp"

namespace GouraudShading
{
    // Phong������ɫ����
    // ʵ����Phong����ģ�ͣ�����������;��淴��       
    class Phong : public Shader
    {
    private:
        Vec3 diffuseColor;   // ��������ɫ
        Vec3 specularColor;  // ���淴����ɫ
        float specularEx;    // ���淴��ָ��

    public:
        // ���캯��
        // material: ���ʲ���
        // textures: ��������
        Phong(Material& material, vector<Texture>& textures);

        // ������ɫ���
        // in: ���䷽��
        // out: ���䷽��
        // normal: ���淨��
        // ������ɫ�������ɫֵ
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const;
    };
}

#endif
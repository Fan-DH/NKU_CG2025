#pragma once
#ifndef __SHADER_CREATOR_HPP__
#define __SHADER_CREATOR_HPP__

// ��ɫ��������ͷ�ļ�
// ���������ڴ���������ɫ���Ĺ�����

#include "Shader.hpp"
#include "Phong.hpp"
#include "Lambertian.hpp"

namespace PhongShading
{
    // ��ɫ����������
    // ���ݲ������ʹ�����Ӧ����ɫ��
    class ShaderCreator
    {
    public:
        // Ĭ�Ϲ��캯��
        ShaderCreator() = default;

        // ������ɫ��
        // material: ���ʲ���
        // t: ��������
        // ���ش�������ɫ��ָ��
        SharedShader create(Material& material, vector<Texture>& t) {
            SharedShader shader{nullptr};
            // ���ݲ�������ѡ����ɫ��
            switch (material.type)
            {
            case 0:  // Lambert
                shader = make_shared<Lambertian>(material, t);
                break;
            case 1:  // Phong
                shader = make_shared<Phong>(material, t);
                break;
            default:  
                shader = make_shared<Lambertian>(material, t);
                break;
            }
            return shader;
        }
    };
}

#endif
#pragma once
#ifndef __SHADER_CREATOR_HPP__
#define __SHADER_CREATOR_HPP__

// ��ɫ��������ͷ�ļ�
// ���������ڴ���������ɫ���Ĺ�����

#include "Shader.hpp"
#include "Lambertian.hpp"
#include "Phong.hpp"
#include "CookTorrance.hpp"

namespace RayCast
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
            case 0:  // Lambert����
                shader = make_shared<Lambertian>(material, t);
                break;
            case 1:  // Phong����
                shader = make_shared<Phong>(material, t);
                break;
            case 2:  
                shader = make_shared<CookTorrance>(material, t);
                break;
            default:  // Ĭ��ʹ��Lambert����
                shader = make_shared<Lambertian>(material, t);
                break;
            }
            return shader;
        }
    };
}

#endif
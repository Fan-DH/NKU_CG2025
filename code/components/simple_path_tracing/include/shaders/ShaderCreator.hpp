#pragma once
#ifndef __SHADER_CREATOR_HPP__
#define __SHADER_CREATOR_HPP__

#include "Shader.hpp"
#include "Lambertian.hpp"

namespace SimplePathTracer
{
    /**
     * ��ɫ��������
     * ���ݲ������ʹ�����Ӧ����ɫ��ʵ��
     */
    class ShaderCreator
    {
    public:
        ShaderCreator() = default;
        
        /**
         * ���ݲ������ʹ�����ɫ��
         * @param material ���ʶ���
         * @param t ��������
         * @return ��ɫ������ָ��
         */
        SharedShader create(Material& material, vector<Texture>& t) {
            SharedShader shader{nullptr};
            switch (material.type)
            {
            case 0:  // Lambertian����
                shader = make_shared<Lambertian>(material, t);
                break;
            default:  // Ĭ��ʹ��Lambertian
                shader = make_shared<Lambertian>(material, t);
                break;
            }
            return shader;
        }
    };
}

#endif
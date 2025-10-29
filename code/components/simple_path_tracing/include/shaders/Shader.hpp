#pragma once
#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include "scene/Scene.hpp"

#include "Scattered.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;

    constexpr float PI = 3.1415926535898f;  // Բ���ʳ���

    /**
     * ��ɫ������
     * �����������߽����Ľӿڣ�ʵ�ֲ�ͬ�Ĺ���ģ��
     */
    class Shader
    {
    protected:
        Material& material;              // ��������
        vector<Texture>& textureBuffer;  // ������������
        
    public:
        /**
         * ���캯��
         * @param material ���ʶ���
         * @param textures ��������
         */
        Shader(Material& material, vector<Texture>& textures)
            : material              (material)
            , textureBuffer         (textures)
        {}
        
        /**
         * �����������ʵĽ������
         * @param ray �������
         * @param hitPoint �ཻ��
         * @param normal ������
         * @return ɢ����Ϣ
         */
        virtual Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const = 0;
    };
    SHARE(Shader);  // ���干��ָ������
}

#endif
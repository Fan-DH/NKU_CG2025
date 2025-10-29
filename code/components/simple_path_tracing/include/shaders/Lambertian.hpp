#pragma once
#ifndef __LAMBERTIAN_HPP__
#define __LAMBERTIAN_HPP__

#include "Shader.hpp"

namespace SimplePathTracer
{
    /**
     * Lambertian������ɫ��
     * ʵ���������������ʣ���ѭLambert���Ҷ���
     * ���������з����Ͼ���ɢ�䣬ɢ��ǿ��������Ƕȵ����ҳ�����
     */
    class Lambertian : public Shader
    {
    private:
        Vec3 albedo;  // �����ʣ���������ɫ��
        
    public:
        /**
         * ���캯��
         * @param material ���ʶ���
         * @param textures ��������
         */
        Lambertian(Material& material, vector<Texture>& textures);
        
        /**
         * ����Lambertianɢ��
         * @param ray �������
         * @param hitPoint �ཻ��
         * @param normal ������
         * @return ɢ����Ϣ
         */
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };
}

#endif
#pragma once
#ifndef __INTERSECTIONS_HPP__
#define __INTERSECTIONS_HPP__

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace RayTracer
{
    /**
     * �����뼸�����ཻ��������ռ�
     * �ṩ���ּ���������ߵ��ཻ����㷨
     */
    namespace Intersection
    {
        /**
         * �������������ཻ���
         * @param ray ����
         * @param t ������
         * @param tMin ��С����ֵ
         * @param tMax ������ֵ
         * @return �ཻ��¼
         */
        HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * �����������ཻ���
         * @param ray ����
         * @param s ����
         * @param tMin ��С����ֵ
         * @param tMax ������ֵ
         * @return �ཻ��¼
         */
        HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * ������ƽ���ཻ���
         * @param ray ����
         * @param p ƽ��
         * @param tMin ��С����ֵ
         * @param tMax ������ֵ
         * @return �ཻ��¼
         */
        HitRecord xPlane(const Ray& ray, const Plane& p, float tMin = 0.f, float tMax = FLOAT_INF);
        
        /**
         * �����������Դ�ཻ���
         * @param ray ����
         * @param a �����Դ
         * @param tMin ��С����ֵ
         * @param tMax ������ֵ
         * @return �ཻ��¼
         */
        HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin = 0.f, float tMax = FLOAT_INF);
    }
}

#endif
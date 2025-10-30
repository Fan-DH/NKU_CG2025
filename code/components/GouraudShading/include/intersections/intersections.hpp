#pragma once
#ifndef __INTERSECTIONS_HPP__
#define __INTERSECTIONS_HPP__

// �ཻ���Ժ���ͷ�ļ�
// �����˸��ּ���������ߵ��ཻ���Ժ���

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace GouraudShading
{
    // �ཻ���������ռ�
    // ���������м�������ཻ���Ժ���
    namespace Intersection
    {
        // �����������ε��ཻ����
        // ray: ����
        // t: ������
        // tMin, tMax: �ཻ���뷶Χ
        // �����ཻ��¼
        HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin = 0.f, float tMax = FLOAT_INF);

        // ������������ཻ����
        // ray: ����
        // s: ����
        // tMin, tMax: �ཻ���뷶Χ
        // �����ཻ��¼
        HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin = 0.f, float tMax = FLOAT_INF);

        // ������ƽ����ཻ����
        // ray: ����
        // p: ƽ��
        // tMin, tMax: �ཻ���뷶Χ
        // �����ཻ��¼
        HitRecord xPlane(const Ray& ray, const Plane& p, float tMin = 0.f, float tMax = FLOAT_INF);

        // ���������Դ���ཻ����
        // ray: ����
        // a: ���Դ
        // tMin, tMax: �ཻ���뷶Χ
        // �����ཻ��¼
        HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin = 0.f, float tMax = FLOAT_INF);
    }
}

#endif
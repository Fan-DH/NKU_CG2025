#pragma once
#ifndef __RAY_HPP__
#define __RAY_HPP__

#include "geometry/vec.hpp"

#include <limits>

#define FLOAT_INF numeric_limits<float>::infinity()
namespace RayTracer
{
    using namespace NRenderer;
    using namespace std;

    /**
     * ���߽ṹ��
     * ��ʾ�����������ض����򴫲��Ĺ���
     */
    struct Ray
    {
        Vec3 origin;        // �������
        Vec3 direction;     // ���߷��򣨱���Ϊ��λ������

        /**
         * ���ù������
         * @param v �µ����
         */
        void setOrigin(const Vec3& v) {
            origin = v;
        }

        /**
         * ���ù��߷���
         * @param v �µķ������������Զ���һ����
         */
        void setDirection(const Vec3& v) {
            direction = glm::normalize(v);
        }

        /**
         * ��������ڲ���t���ĵ�
         * @param t ����ֵ
         * @return �����ϵĵ�
         */
        inline
        Vec3 at(float t) const {
            return origin + t*direction;
        }

        /**
         * ���캯��
         * @param origin �������
         * @param direction ���߷���
         */
        Ray(const Vec3& origin, const Vec3& direction)
            : origin                (origin)
            , direction             (direction)
        {}
    
        /**
         * Ĭ�Ϲ��캯��
         */
        Ray()
            : origin        {}
            , direction     {}
        {}
    };
}

#endif
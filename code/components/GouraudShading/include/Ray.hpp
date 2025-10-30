#pragma once
#ifndef __RAY_HPP__
#define __RAY_HPP__

// ������ͷ�ļ�
// �����˹��ߵĻ������ԺͲ���

#include "geometry/vec.hpp"
#include <limits>

#define FLOAT_INF numeric_limits<float>::infinity()

namespace GouraudShading
{
    using namespace NRenderer;
    using namespace std;

    // ���߽ṹ��
    // �������ߵ����ͷ���
    struct Ray
    {
        Vec3 origin;      // �������
        Vec3 direction;   // ���߷��򣨵�λ������

        // ���ù������
        // v: �µ����λ��
        void setOrigin(const Vec3& v) {
            origin = v;
        }

        // ���ù��߷���
        // v: �µķ����������ᱻ��һ����
        void setDirection(const Vec3& v) {
            direction = glm::normalize(v);
        }

        // ��������ϵĵ�
        // t: ����ֵ�����룩
        // ���ع����Ͼ������t����ĵ�
        inline
        Vec3 at(float t) const {
            return origin + t*direction;
        }

        // ���캯��
        // origin: �������
        // direction: ���߷���
        Ray(const Vec3& origin, const Vec3& direction)
            : origin                (origin)
            , direction             (direction)
        {}
    
        // Ĭ�Ϲ��캯��
        // ����һ�����ͷ���Ϊ�������Ĺ���
        Ray()
            : origin        {}
            , direction     {}
        {}
    };
}

#endif
// ����ඨ��
// �����˳����е�������Ժ���Ϊ��֧�־���Ч��
#pragma once
#ifndef __NR_CAMERA_HPP__
#define __NR_CAMERA_HPP__

#include <memory>
#include "geometry/vec.hpp"

namespace NRenderer
{
    using namespace std;
    
    // ����ṹ��
    // �����������λ�á������ӳ��ǵ����ԣ�֧�־���Ч����ģ��
    struct Camera
    {
        Vec3 position;       // ���λ��
        Vec3 up;            // ����Ϸ���
        Vec3 lookAt;        // ���ע�ӵ�
        float fov;          // �ӳ��ǣ��ȣ�
        float aperture;     // ��Ȧ��С�����ھ���Ч����
        float focusDistance;// ���ࣨ���ھ���Ч����
        float aspect;       // ��߱�

        // Ĭ�Ϲ��캯��
        // ����һ��λ��(0,0,-3)��������ԭ������
        Camera() noexcept
            : position      (0.f, 0.f, -3.f)
            , up            (0.f, 1.f, 0.f)
            , lookAt        (0.f, 0.f, 0.f)
            , fov           (40)
            , aperture      (0.f)
            , focusDistance (0.1f)
            , aspect        (1.f)
        {}

        // �������Ĺ��캯��
        Camera(
            const Vec3& position,    // ���λ��
            const Vec3& up,          // ����Ϸ���
            const Vec3& lookAt,      // ���ע�ӵ�
            float fov,               // �ӳ��ǣ��ȣ�
            float aperture,          // ��Ȧ��С
            float focusDistance,     // ����
            float aspect            // ��߱�
        )
            : position      (position)
            , up            (up)
            , lookAt        (lookAt)
            , fov           (fov)
            , aperture      (aperture)
            , focusDistance (focusDistance)
            , aspect        (aspect)
        {}
    };
    using SharedCamera = shared_ptr<Camera>;    // ���������ָ������
}

#endif

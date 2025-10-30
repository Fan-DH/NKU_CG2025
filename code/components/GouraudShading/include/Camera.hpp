#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

// �����ͷ�ļ�
// ʵ���˹���Ͷ����Ⱦ�е����ģ�ͣ�֧�־���Ч��

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"
#include "Ray.hpp"

namespace GouraudShading
{
    using namespace std;
    using namespace NRenderer;

    // �����
    // �������ɴ����λ�÷���Ĺ���
    class Camera
    {
    private:
        const NRenderer::Camera& camera;  // �����������
        float lenRadius;                  // ��ͷ�뾶�����ھ���Ч����
        Vec3 u, v, w;                    // �������ϵ������
        Vec3 vertical;                    // ��ƽ�洹ֱ��������
        Vec3 horizontal;                  // ��ƽ��ˮƽ��������
        Vec3 lowerLeft;                  // ��ƽ�����½�λ��
        Vec3 position;                   // ���λ��

    public:
        // ���캯��
        // camera: �����������
        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            // ���㾵ͷ�뾶�����ھ��
            lenRadius = camera.aperture / 2.f;
            // �����ӳ���
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            auto theta = glm::radians(vfov);
            auto halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;

            // �����������ϵ
            Vec3 up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt);  // �۲췽��ķ�����
            u = glm::normalize(glm::cross(up, w));               // �ҷ���
            v = glm::cross(w, u);                                // �Ϸ���

            // ���㽹��
            auto focusDis = camera.focusDistance;

            // ������ƽ���λ�úʹ�С
            lowerLeft = position - halfWidth*focusDis*u
                - halfHeight*focusDis*v
                - focusDis*w;
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
        }

        // ������������
        // s: ��һ����ˮƽ���� [0,1]
        // t: ��һ���Ĵ�ֱ���� [0,1]
        // �������ɵĹ���
        Ray shoot(float s, float t) const {
            return Ray{
                position,
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position
                )
            };
        }
    };
}

#endif
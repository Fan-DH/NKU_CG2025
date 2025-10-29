#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"

#include "samplers/SamplerInstance.hpp"

#include "Ray.hpp"

namespace SimplePathTracer
{
    using namespace std;
    using namespace NRenderer;
    
    /**
     * ��·��׷�������
     * ��������λ�÷�����ߣ�֧�־���Ч��
     */
    class Camera
    {
    private:
        const NRenderer::Camera& camera;  // ԭʼ�����������
        float lenRadius;                  // ��ͷ�뾶�����ھ���Ч��
        Vec3 u, v, w;                     // �������������������
        Vec3 vertical;                    // ��ֱ��������
        Vec3 horizontal;                  // ˮƽ��������
        Vec3 lowerLeft;                   // ����ƽ�����½�λ��
        Vec3 position;                    // ���λ��
    public:
        /**
         * ���캯������ʼ���������
         * @param camera ԭʼ�������
         */
        Camera(const NRenderer::Camera& camera)
            : camera                (camera)
        {
            position = camera.position;
            lenRadius = camera.aperture / 2.f;  // ���㾵ͷ�뾶
            
            // �����ӳ��ǣ�������20-160��֮��
            auto vfov = camera.fov;
            vfov = clamp(vfov, 160.f, 20.f);
            auto theta = glm::radians(vfov);
            auto halfHeight = tan(theta/2.f);
            auto halfWidth = camera.aspect*halfHeight;
            
            // �����������ϵ
            Vec3 up = camera.up;
            w = glm::normalize(camera.position - camera.lookAt);  // �������
            u = glm::normalize(glm::cross(up, w));                // �ҷ���
            v = glm::cross(w, u);                                 // �Ϸ���

            // �������ƽ�����
            auto focusDis = camera.focusDistance;
            lowerLeft = position - halfWidth*focusDis*u
                - halfHeight*focusDis*v
                - focusDis*w;
            horizontal = 2*halfWidth*focusDis*u;
            vertical = 2*halfHeight*focusDis*v;
        }

        /**
         * ��������������
         * @param s ˮƽ������� [0,1]
         * @param t ��ֱ������� [0,1]
         * @return ����Ĺ���
         */
        Ray shoot(float s, float t) const {
            // ʹ��Բ�β��������ɾ�ͷƫ�ƣ�ʵ�־���Ч��
            auto r = defaultSamplerInstance<UniformInCircle>().sample2d();
            float rx = r.x * lenRadius;
            float ry = r.y * lenRadius;
            Vec3 offset = u*rx + v*ry;
            
            // ����������ͷ���
            return Ray{
                position + offset,  // ������㣨���Ǿ�ͷƫ�ƣ�
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position - offset
                )
            };
        }
    };
}

#endif
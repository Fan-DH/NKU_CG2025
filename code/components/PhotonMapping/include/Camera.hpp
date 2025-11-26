#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"


#include "Ray.hpp"

namespace PhotonMapping
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
            Vec3 up = -camera.up;
            w = glm::normalize(camera.position - camera.lookAt);
            u = glm::normalize(glm::cross(w, up));
            v = glm::cross(u, w);

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
            // Whitted：不进行随机镜头采样，生成确定性主光线
            Vec3 offset{0.0f};
            return Ray{
                position,
                glm::normalize(
                    lowerLeft + s*horizontal + t*vertical - position - offset
                )
            };
        }
    };
}

#endif
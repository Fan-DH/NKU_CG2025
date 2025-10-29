#pragma once
#ifndef __ONB_HPP__
#define __ONB_HPP__

#include "geometry/vec.hpp"

namespace SimplePathTracer
{
    using namespace NRenderer;
    
    /**
     * ��������Orthonormal Basis����
     * �����ھֲ�����ϵ����������ϵ֮�����ת��
     * ��Ҫ���ڽ�����������ת�����Է�����Ϊz��ľֲ�����ϵ
     */
    class Onb
    {
    private:
        Vec3 u;  // �ֲ�����ϵ��x��
        Vec3 v;  // �ֲ�����ϵ��y��
        Vec3 w;  // �ֲ�����ϵ��z�ᣨ����������
        
    public:
        /**
         * ���캯�������ݷ���������������
         * @param normal ������������Ϊ�ֲ�����ϵ��z��
         */
        Onb(const Vec3& normal) {
            w = normal;  // z��Ϊ����������
            
            // ѡ��һ���뷨������ƽ�е�������Ϊ�ο�
            Vec3 a = (fabs(w.x) > 0.9) ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
            
            // ͨ���������y���x��
            v = glm::normalize(glm::cross(w, a));  // y��
            u = glm::cross(w, v);                  // x��
        }
        
        ~Onb() = default;

        /**
         * ���ֲ�����ϵ������ת������������ϵ
         * @param v �ֲ�����ϵ�е�����
         * @return ��������ϵ�е�����
         */
        Vec3 local(const Vec3& v) const {
            return v.x*this->u + v.y*this->v + v.z * this->w;
        }
    };
}


#endif
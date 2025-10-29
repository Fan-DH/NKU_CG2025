#pragma once
#ifndef __SCATTERED_HPP__
#define __SCATTERED_HPP__

#include "Ray.hpp"

namespace SimplePathTracer
{
    /**
     * ɢ����Ϣ�ṹ��
     * ��¼��������ʽ������ɢ����Ϣ
     */
    struct Scattered
    {
        Ray ray = {};           // ɢ���Ĺ��߷���
        Vec3 attenuation = {};  // ˥��ϵ����BRDF��
        Vec3 emitted = {};      // �Է���ǿ��
        float pdf = {0.f};      // �����ܶȺ���ֵ
    };
    
}

#endif
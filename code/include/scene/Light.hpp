// ��Դ���Ͷ���
// �����˳����еĸ��ֹ�Դ���ͣ��������Դ���۹�ơ�ƽ�й�������
#pragma once
#ifndef __NR_LIGHT_HPP__
#define __NR_LIGHT_HPP__

#include "geometry/vec.hpp"
#include <numbers>

#include <memory>
#include <string>

#include "Model.hpp"

namespace NRenderer
{
    // ������Դ�ṹ��
    // �����˹�Դ�Ļ������Ժ�����
    struct Light
    {
        // ��Դ����ö��
        enum class Type
        {
            POINT = 0x0,                // ���Դ����һ���������з��������
            SPOT,                       // �۹�ƣ���һ�������ض�������׶�ι���
            DIRECTIONAL,                // ƽ�й⣺���й���ƽ�У�ģ��Զ����Դ����̫����
            AREA                        // ����⣺��һ�����������ߣ���������Ӱ
        };
        Type type;        // ��Դ����
        Index entity;     // ��Դʵ������������ڹ�������Ĺ�Դ����

        // ���캯��
        Light(Type type)
            : type                  (type)
            , entity                (0)
        {}
        Light() = default;
    };
    SHARE(Light);

    // �����Դ
    // ������һ�����������Դ�����Բ�������ӰЧ��
    struct AreaLight
    {
        Vec3 radiance = {1, 1, 1};  // ����ȣ���Դǿ�ȣ�
        Vec3 position = {};         // ��Դλ�ã����ε�һ���ǵ㣩
        Vec3 u = {};               // ���εĵ�һ��������
        Vec3 v = {};               // ���εĵڶ���������
    };
    SHARE(AreaLight);

    // ���Դ
    // ��һ���������з�����ȷ������
    struct PointLight
    {
        Vec3 intensity = {1, 1, 1};  // ��Դǿ��
        Vec3 position = {};          // ��Դλ��
    };
    SHARE(PointLight);

    // ƽ�й�Դ
    // ���й���ƽ�У�����ģ��Զ���Ĺ�Դ
    struct DirectionalLight
    {
        Vec3 irradiance = {1, 1, 1}; // ���ն�
        Vec3 direction = {};         // ���߷���
    };
    SHARE(DirectionalLight);

    // �۹��
    // ��һ�������ض�������׶�ι���
    struct SpotLight
    {
        Vec3 intensity = {1, 1, 1};  // ��Դǿ��
        Vec3 position = {};          // ��Դλ��
        Vec3 direction = {};         // ���շ���
        float hotSpot = 3.1415926f/4.f;  // �ȵ�Ƕȣ���׶�ǣ�
        float fallout = 3.1415926f/3.f;  // ˥���Ƕȣ���׶�ǣ�
    };
    SHARE(SpotLight);
} // namespace Light


#endif
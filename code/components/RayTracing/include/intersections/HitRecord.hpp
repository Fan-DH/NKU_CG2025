#pragma once
#ifndef __HIT_RECORD_HPP__
#define __HIT_RECORD_HPP__

#include <optional>

#include "geometry/vec.hpp"

namespace RayTracer
{
    using namespace NRenderer;
    using namespace std;
    
    /**
     * �����������ཻ��¼�Ļ����ṹ
     * ��¼�����������ཻʱ����ϸ��Ϣ
     */
    struct HitRecordBase
    {
        float t;           // ���߲���t����ʾ�ཻ�����������ľ���
        Vec3 hitPoint;     // �ཻ�����������
        Vec3 normal;       // �ཻ�㴦�ķ�����
        Handle material;   // �ཻ����Ĳ��ʾ��
    };
    
    // ʹ��optional��װ����ʾ����û���ཻ
    using HitRecord = optional<HitRecordBase>;
    
    /**
     * ����δ�ཻ��¼
     * @return ��ʾδ�ཻ��HitRecord
     */
    inline
    HitRecord getMissRecord() {
        return nullopt;
    }

    /**
     * �����ཻ��¼
     * @param t ���߲���
     * @param hitPoint �ཻ��
     * @param normal ������
     * @param material ���ʾ��
     * @return �����ཻ��Ϣ��HitRecord
     */
    inline
    HitRecord getHitRecord(float t, const Vec3& hitPoint, const Vec3& normal, Handle material) {
        return make_optional<HitRecordBase>(t, hitPoint, normal, material);
    }
}

#endif
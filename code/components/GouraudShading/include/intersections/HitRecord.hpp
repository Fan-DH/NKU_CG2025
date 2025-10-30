#pragma once
#ifndef __HIT_RECORD_HPP__
#define __HIT_RECORD_HPP__

// �ཻ��¼ͷ�ļ�
// �����˹����������ཻ�ļ�¼�ṹ

#include <optional>

#include "geometry/vec.hpp"

namespace GouraudShading
{
    using namespace NRenderer;
    using namespace std;

    // �ཻ��¼�����ṹ
    // �洢�����������ཻ����ϸ��Ϣ
    struct HitRecordBase
    {
        float t;         // �ཻ����
        Vec3 hitPoint;   // �ཻ��
        Vec3 normal;     // �ཻ�㷨��
        Handle material; // �ཻ����Ĳ���
    };

    // �ཻ��¼����
    // ʹ��optional��װ�����Ա�ʾ�Ƿ����ཻ
    using HitRecord = optional<HitRecordBase>;

    // ��ȡδ�ཻ��¼
    // ����һ����ʾδ�ཻ�ļ�¼
    inline
    HitRecord getMissRecord() {
        return nullopt;
    }

    // �����ཻ��¼
    // t: �ཻ����
    // hitPoint: �ཻ��
    // normal: �ཻ�㷨��
    // material: �ཻ����Ĳ���
    // ����һ�������ཻ��Ϣ�ļ�¼
    inline
    HitRecord getHitRecord(float t, const Vec3& hitPoint, const Vec3& normal, Handle material) {
        return make_optional<HitRecordBase>(t, hitPoint, normal, material);
    }
}

#endif
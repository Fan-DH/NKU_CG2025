#pragma once
#ifndef __NR_LIGHT_ITEM_HPP__
#define __NR_LIGHT_ITEM_HPP__

// ��Դ�ʲ���ͷ�ļ�
// �����˳����й�Դ�����ݽṹ

#include "scene/Light.hpp"
#include "Item.hpp"

namespace NRenderer
{
    using namespace std;

    // ��Դ�ʲ���
    // �����˹�Դ��OpenGL��Ⱦ���ݺ͹�Դ����
    struct LightItem : public Item
    {
        GlId glVAO{0};  // �����������
        GlId glVBO{0};  // ���㻺�����
        GlId glEBO{0};  // �����������
        SharedLight light;  // ��Դ���ݵ�����ָ��
        SharedGlDrawData externalDrawData;  // �ⲿ�������ݣ����ڿ��ӻ���Դ��
    };
    
} // namespace NRenderer

#endif
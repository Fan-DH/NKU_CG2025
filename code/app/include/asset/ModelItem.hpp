#pragma once
#ifndef __NR_MODEL_ITEM_HPP__
#define __NR_MODEL_ITEM_HPP__

// ģ���ʲ���ͷ�ļ�
// �����˳�����ģ�ͺͽڵ�����ݽṹ

#include "scene/Model.hpp"
#include "Item.hpp"

#include "glad/glad.h"

namespace NRenderer
{
    using namespace std;

    // ģ���ʲ���
    // ������һ������ģ�͵�����
    struct ModelItem : public Item
    {
        SharedModel model{nullptr};  // ģ�����ݵ�����ָ��
    };

    // �ڵ��ʲ���
    // ������ģ�ͽڵ��OpenGL��Ⱦ����
    struct NodeItem : public Item
    {
        GlId glVAO{0};  // �����������
        GlId glVBO{0};  // ���㻺�����
        GlId glEBO{0};  // �����������
        SharedNode node{nullptr};  // �ڵ����ݵ�����ָ��
        SharedGlDrawData externalDrawData{nullptr};  // �ⲿ��������
    };
    
} // namespace NRenderer

#endif
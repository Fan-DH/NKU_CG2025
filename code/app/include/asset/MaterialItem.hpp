#pragma once
#ifndef __NR_MATERIAL_ITEM_HPP__
#define __NR_MATERIAL_ITEM_HPP__

// �����ʲ���ͷ�ļ�
// �����˳����в��ʵ����ݽṹ

#include "scene/Material.hpp"
#include "Item.hpp"

namespace NRenderer
{
    // �����ʲ���
    // ������һ���������ʵ�����
    struct MaterialItem : public Item
    {
        SharedMaterial material;  // �������ݵ�����ָ��
    };
} // namespace NRenderer

#endif
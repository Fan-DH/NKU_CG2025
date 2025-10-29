#pragma once
#ifndef __NR_TEXTURE_ITEM_HPP__
#define __NR_TEXTURE_ITEM_HPP__

// �����ʲ���ͷ�ļ�
// �����˳�������������ݽṹ

#include "scene/Texture.hpp"
#include "Item.hpp"
#include "utilities/GlImage.hpp"

namespace NRenderer
{
    // �����ʲ���
    // �������������ݺͶ�Ӧ��OpenGL�������
    struct TextureItem : public Item
    {
        SharedTexture texture;  // �������ݵ�����ָ��
        GlImageId glId;        // OpenGL�������ID
    };
} // namespace NRenderer

#endif
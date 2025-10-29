#include "importer/TextureImporter.hpp"
#include "utilities/ImageLoader.hpp"
#include "utilities/GlImage.hpp"
#include "asset/TextureItem.hpp"

// ��������ʵ���ļ�
// ������ļ��е�����������OpenGL�������

namespace NRenderer
{
    // ���������ļ�
    // asset: �ʲ�������
    // path: �����ļ�·��
    // ����ֵ: �����Ƿ�ɹ�
    bool TextureImporter::import(Asset& asset, const string& path) {
        ImageLoader imgLoader;
        auto img = imgLoader.load(path);                // ����ͼ���ļ�

        // �����������
        SharedTexture spTexture{new Texture()};
        spTexture->width = img->width;                 // ����������
        spTexture->height = img->height;               // ��������߶�
        spTexture->rgba = (RGBA*)img->data;           // ������������

        // ����OpenGL����
        auto id = GlImage::loadImage(spTexture->rgba, {spTexture->width, spTexture->height});

        // ����������
        TextureItem ti;
        ti.glId = id;                                 // OpenGL����ID
        ti.name = path;                               // �������ƣ��ļ�·����
        ti.texture = spTexture;                       // ��������
        asset.textureItems.push_back(move(ti));       // ��ӵ��ʲ�������

        return true;
    }
}
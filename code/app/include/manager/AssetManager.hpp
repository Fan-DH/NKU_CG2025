#pragma once
#ifndef __NR_ASSET_MANAGER_HPP__
#define __NR_ASSET_MANAGER_HPP__

// �ʲ�������ͷ�ļ�
// �����˹������ʲ��Ĺ�����

#include "asset/Asset.hpp"
#include "importer/TextureImporter.hpp"
#include "utilities/FileFetcher.hpp"
#include "importer/SceneImporterFactory.hpp"
#include "utilities/File.hpp"
#include "server/Server.hpp"

namespace NRenderer
{
    // �ʲ��������ṹ��
    // �������Ͳ��������е������ʲ�
    struct AssetManager
    {
        Asset asset;  // �����ʲ�ʵ��

        // ���볡���ļ�
        // ֧�ֵ��� .scn �� .obj ��ʽ�ĳ����ļ�
        void importScene() {
            FileFetcher ff;
            auto optPath = ff.fetch("All\0*.scn;*.obj\0");
            if (optPath) {
                auto importer = SceneImporterFactory::instance().importer(File::getFileExtension(*optPath));
                bool success = importer->import(asset, *optPath);
                if (!success) {
                    getServer().logger.error(importer->getErrorInfo());
                }
                else {
                    getServer().logger.success("�ɹ�����:" + *optPath);
                }
            }
        }

        // ���������ļ�
        // ֧�ֵ��� .png �� .jpg ��ʽ��ͼƬ�ļ�
        void importTexture() {
            TextureImporter tImp{};
            FileFetcher ff;
            auto optPath = ff.fetch("image\0*.png;*.jpg\0");
            if (optPath) {
                tImp.import(asset, *optPath);
            }
        }

        // ��������ʲ�
        // ����ģ�͡���Դ�����ʺ�����
        void clearAll() {
            asset.clearModel();
            asset.clearLight();
            asset.clearMaterial();
            asset.clearTexture();
        }

        // ���ģ���ʲ�
        void clearModel() {
            asset.clearModel();
        }

        // �����Դ�ʲ�
        void clearLight() {
            asset.clearLight();
        }
    };
}


#endif

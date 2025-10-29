#pragma once
#ifndef __NR_ASSET_HPP__
#define __NR_ASSET_HPP__

// �ʲ�����ͷ�ļ�
// �����˳����������ʲ������ݽṹ�͹�����

#include "common/macros.hpp"

#include "ModelItem.hpp"
#include "MaterialItem.hpp"
#include "TextureItem.hpp"
#include "LightItem.hpp"

namespace NRenderer
{
    // �ʲ��ṹ��
    // �����˳��������е�ģ�͡����ʡ�����͹�Դ����
    struct Asset
    {
        // �����ʲ���
        vector<ModelItem> modelItems;         // ģ�����б�
        vector<NodeItem> nodeItems;           // �ڵ����б�
        vector<MaterialItem> materialItems;    // �������б�
        vector<TextureItem> textureItems;     // �������б�
        vector<LightItem> lightItems;         // ��Դ���б�

        // �������ʲ�
        vector<SharedSphere> spheres;         // �����б�
        vector<SharedTriangle> triangles;     // �������б�
        vector<SharedPlane> planes;           // ƽ���б�
        vector<SharedMesh> meshes;            // �����б�

        // ��Դ�ʲ�
        vector<SharedPointLight> pointLights;          // ���Դ�б�
        vector<SharedAreaLight> areaLights;           // ���Դ�б�
        vector<SharedDirectionalLight> directionalLights;  // ƽ�й��б�
        vector<SharedSpotLight> spotLights;           // �۹���б�

        // ���ģ������
        // ɾ������ģ����ص�OpenGL�����������ģ���б�
        void clearModel() {
            for (auto& node : nodeItems) {
                if (node.glVAO != 0) {
                    glDeleteVertexArrays(1, &node.glVAO);
                }
                if (node.glVBO != 0) {
                    glDeleteBuffers(1, &node.glVBO);
                }
                if (node.glEBO != 0) {
                    glDeleteBuffers(1, &node.glEBO);
                }
            }
            for (auto& light : lightItems) {
                if (light.glVAO != 0) {
                    glDeleteVertexArrays(1, &light.glVAO);
                    light.glVAO = 0;
                }
                if (light.glVBO != 0) {
                    glDeleteBuffers(1, &light.glVBO);
                    light.glVBO = 0;
                }
                if (light.glEBO != 0) {
                    glDeleteBuffers(1, &light.glEBO);
                    light.glEBO = 0;
                }
            }
            modelItems.clear();
            nodeItems.clear();

            spheres.clear();
            triangles.clear();
            planes.clear();
            meshes.clear();
        }

        // �����Դ����
        // ������й�Դ�б�
        void clearLight() {
            lightItems.clear();

            pointLights.clear();
            areaLights.clear();
            directionalLights.clear();
            spotLights.clear();
        }

        // �����������
        // ��ղ����б�
        void clearMaterial() {
            materialItems.clear();
        }

        // �����������
        // ��������б�
        void clearTexture() {
            textureItems.clear();
        }

        // Ϊ�ڵ�����Ԥ���õ�OpenGL������
        void genPreviewGlBuffersPerNode(NodeItem& node);
        // Ϊ��Դ����Ԥ���õ�OpenGL������
        void genPreviewGlBuffersPerLight(LightItem& light);

        // ���½ڵ��OpenGL��������
        void updateNodeGlDrawData(NodeItem& node);
        // ���¹�Դ��OpenGL��������
        void updateLightGlDrawData(LightItem& light);
    };
    
} // namespace NRenderer


#endif
#include "asset/SceneBuilder.hpp"

// ����������ʵ���ļ�
// �����ʲ��������е����ݹ�������Ⱦ����

namespace NRenderer
{
    // ������Ⱦѡ��
    // ������Ⱦ��ȡ����������ֱ��ʵȲ���
    void SceneBuilder::buildRenderOption() {
        RenderOption ro;
        ro.depth = renderSettings.depth;                    // ����׷�����
        ro.samplesPerPixel = renderSettings.samplesPerPixel;  // ÿ���ز�����
        ro.width = renderSettings.width;                    // ��Ⱦ����
        ro.height = renderSettings.height;                  // ��Ⱦ�߶�
        ro.useKDTree = renderSettings.useKDTree;            // KD-Tree 加速开关（仅 RayTracing 有效）
        this->scene->renderOption = ro;
    }

    // ��������������
    // �������ʲ����ݸ��Ƶ�������
    void SceneBuilder::buildBuffer() {
        // ���Ʋ��ʡ�������ģ������
        for (auto& mi : asset.materialItems) {
            this->scene->materials.push_back(*mi.material);
        }
        for (auto& ti : asset.textureItems) {
            this->scene->textures.push_back(*ti.texture);
        }
        for (auto& mi : asset.modelItems) {
            this->scene->models.push_back(*mi.model);
        }

        // ���ƽڵ����ݲ���������Ч��
        for (auto& ni : asset.nodeItems) {
            this->scene->nodes.push_back(*ni.node);
            if (ni.node->type == Node::Type::SPHERE) {
                if (!asset.spheres[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::PLANE) {
                if (!asset.planes[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::TRIANGLE) {
                if (!asset.triangles[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
            if (ni.node->type == Node::Type::MESH) {
                if (!asset.meshes[ni.node->entity]->material.valid()) {
                    success = false;
                    string err{};
                    err = "No material is specified for node "+ni.name+".";
                }
            }
        }

        // ���ƹ�Դ����
        for (auto& li : asset.lightItems) {
            this->scene->lights.push_back(*li.light);
        }

        // ���Ƽ���������
        for (auto& s : asset.spheres) {
            this->scene->sphereBuffer.push_back(*s);
        }
        for (auto& t : asset.triangles) {
            this->scene->triangleBuffer.push_back(*t);
        }
        for (auto& p : asset.planes) {
            this->scene->planeBuffer.push_back(*p);
        }
        for (auto& m : asset.meshes) {
            this->scene->meshBuffer.push_back(*m);
        }

        // ���Ƹ����Դ����
        for (auto& p : asset.pointLights) {
            this->scene->pointLightBuffer.push_back(*p);
        }
        for (auto& a : asset.areaLights) {
            this->scene->areaLightBuffer.push_back(*a);
        }
        for (auto& d : asset.directionalLights) {
            this->scene->directionalLightBuffer.push_back(*d);
        }
        for (auto& s : asset.spotLights) {
            this->scene->spotLightBuffer.push_back(*s);
        }
    }

    // �������
    // ��������ø��Ƶ�������
    void SceneBuilder::buildCamera() {
        this->scene->camera = this->camera;
    }

    // ����������
    // ���û��������ͺͲ���
    void SceneBuilder::buildAmbient() {
        this->scene->ambient.constant = ambientSettings.ambient;          // �����ⳣ��
        this->scene->ambient.environmentMap = ambientSettings.mapTexture;  // ������ͼ
        if (ambientSettings.type == AmbientSettings::Type::CONSTANT) {
            this->scene->ambient.type = Ambient::Type::CONSTANT;          // ����������
        }
        else {
            this->scene->ambient.type = Ambient::Type::ENVIROMENT_MAP;    // ������ͼ����
        }
    }

    // ������������
    // ���ع����õĳ�������
    SharedScene SceneBuilder::build() {
        this->scene = make_shared<Scene>();  // �����³���
        this->buildRenderOption();           // ������Ⱦѡ��
        this->buildCamera();                 // �������
        this->buildBuffer();                 // ��������������
        this->buildAmbient();               // ����������
        if (success)
            return this->scene;              // �����ɹ����س���
        else 
            return nullptr;                  // ����ʧ�ܷ��ؿ�
    }
}
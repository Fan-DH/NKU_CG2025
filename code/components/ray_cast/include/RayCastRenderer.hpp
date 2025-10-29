#pragma once
#ifndef __RAY_CAST_HPP__
#define __RAY_CAST_HPP__

// ����Ͷ����Ⱦ��ͷ�ļ�
// �����˹���Ͷ����Ⱦ���ĺ��Ĺ���

#include "scene/Scene.hpp"
#include "Camera.hpp"
#include "intersections/intersections.hpp"
#include "shaders/ShaderCreator.hpp"

namespace RayCast
{
    using namespace NRenderer;

    // ����Ͷ����Ⱦ����
    // ʵ���˻����Ĺ���Ͷ����Ⱦ�㷨
    class RayCastRenderer
    {
    private:
        SharedScene spScene;                    // ����ָ��
        Scene& scene;                           // ��������
        RayCast::Camera camera;                 // ���
        vector<SharedShader> shaderPrograms;    // ��ɫ�������б�

    public:
        // ���캯��
        // spScene: ����ָ��
        RayCastRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {}

        // ��������
        ~RayCastRenderer() = default;

        // ��Ⱦ������ͣ��������ݡ���ȡ��߶�
        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;

        // ��Ⱦ����
        // ������Ⱦ���
        RenderResult render();

        // �ͷ���Ⱦ���
        // r: Ҫ�ͷŵ���Ⱦ���
        void release(const RenderResult& r);

    private:
        // ٤��У��
        // rgb: �����RGB��ɫ
        // ����У�������ɫ
        RGB gamma(const RGB& rgb);

        // ����׷��
        // r: �������
        // ���ظù��߶�Ӧ����ɫ
        RGB trace(const Ray& r);

        // ����������ཻ����
        // r: �������
        // ����������ཻ��¼
        HitRecord closestHit(const Ray& r);
    };
}

#endif
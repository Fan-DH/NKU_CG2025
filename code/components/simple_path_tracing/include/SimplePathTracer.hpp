#pragma once
#ifndef __SIMPLE_PATH_TRACER_HPP__
#define __SIMPLE_PATH_TRACER_HPP__

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"

#include "shaders/ShaderCreator.hpp"

#include <tuple>
namespace SimplePathTracer
{
    using namespace NRenderer;
    using namespace std;

    /**
     * ��·��׷����Ⱦ��
     * ʵ�ֻ������ؿ��巽����·��׷���㷨
     * ֧�ֶ��ּ������Lambertian����
     */
    class SimplePathTracerRenderer
    {
    public:
    private:
        SharedScene spScene;        // ��������ָ��
        Scene& scene;               // ��������

        unsigned int width;         // ͼ����
        unsigned int height;        // ͼ��߶�
        unsigned int depth;         // ���ݹ����
        unsigned int samples;       // ÿ���ز�����

        using SCam = SimplePathTracer::Camera;
        SCam camera;                // �������

        vector<SharedShader> shaderPrograms;  // ��ɫ�������б�
        
    public:
        /**
         * ���캯��
         * @param spScene ��������ָ��
         */
        SimplePathTracerRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {
            width = scene.renderOption.width;
            height = scene.renderOption.height;
            depth = scene.renderOption.depth;
            samples = scene.renderOption.samplesPerPixel;
        }
        ~SimplePathTracerRenderer() = default;

        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;  // ��Ⱦ�������
        
        /**
         * ִ����Ⱦ
         * @return ��Ⱦ������������ݡ���ȡ��߶ȣ�
         */
        RenderResult render();
        
        /**
         * �ͷ���Ⱦ����ڴ�
         * @param r ��Ⱦ���
         */
        void release(const RenderResult& r);

    private:
        /**
         * ��Ⱦ���񣨶��̣߳�
         * @param pixels ���ػ�����
         * @param width ͼ����
         * @param height ͼ��߶�
         * @param off ��ʼƫ��
         * @param step ����
         */
        void renderTask(RGBA* pixels, int width, int height, int off, int step);

        /**
         * GammaУ��
         * @param rgb ԭʼ��ɫ
         * @return У�������ɫ
         */
        RGB gamma(const RGB& rgb);
        
        /**
         * ·��׷��������
         * @param ray ����
         * @param currDepth ��ǰ�ݹ����
         * @return ������ɫ
         */
        RGB trace(const Ray& ray, int currDepth);
        
        /**
         * ��������ཻ������
         * @param r ����
         * @return �ཻ��¼
         */
        HitRecord closestHitObject(const Ray& r);
        
        /**
         * ��������ཻ�Ĺ�Դ
         * @param r ����
         * @return �ཻ����ͷ���ǿ��
         */
        tuple<float, Vec3> closestHitLight(const Ray& r);
    };
}

#endif
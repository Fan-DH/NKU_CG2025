#pragma once
#ifndef __RAY_TRACER_HPP__
#define __RAY_TRACER_HPP__

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"
// KD-Tree acceleration structure
#include "KDTree.hpp"

#include <tuple>
namespace RayTracer
{
    using namespace NRenderer;
    using namespace std;

    /**
     * 光线追踪渲染器
     * 实现基于路径追踪思想的简化递归着色，当前支持 Lambertian
     */
    class RayTracerRenderer
    {
    private:
        SharedScene spScene;        // 场景指针
        Scene& scene;               // 场景引用

        unsigned int width;         // 图像宽度
        unsigned int height;        // 图像高度
        unsigned int depth;         // 递归深度
        unsigned int samples;       // 每像素采样数

        using Cam = RayTracer::Camera;
        Cam camera;                 // 相机

        // 本实现直接在 trace 中进行局部着色与递归，无需着色器程序表
        bool useKDTree = false;                 // 是否启用KD-Tree加速（仅RayTracing使用）
        std::unique_ptr<KDTree> kdTree;         // KD-Tree指针
        
    public:
        /** 构造函数 */
        RayTracerRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {
            width = scene.renderOption.width;
            height = scene.renderOption.height;
            depth = scene.renderOption.depth;
            samples = scene.renderOption.samplesPerPixel;
        }
        ~RayTracerRenderer() = default;

        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;  // 渲染结果
        
        /** 执行渲染 */
        RenderResult render();
        
        /** 释放渲染结果内存 */
        void release(const RenderResult& r);

    private:
        /** 渲染任务（多线程） */
        void renderTask(RGBA* pixels, int width, int height, int off, int step);

        /** Gamma 校正 */
        RGB gamma(const RGB& rgb);
        
        /** 路径追踪 */
        RGB trace(const Ray& ray, int currDepth);

        /** 查询与物体的最近相交 */
        HitRecord closestHitObject(const Ray& r);
        
        /** 查询与光源的最近相交 */
        tuple<float, Vec3> closestHitLight(const Ray& r);

        /** 构建加速结构（KD-Tree） */
        void buildAcceleration();
    };
}

#endif
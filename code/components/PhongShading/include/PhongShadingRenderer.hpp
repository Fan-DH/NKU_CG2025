#pragma once
#ifndef __PHONG_SHADING_RENDERER_HPP__
#define __PHONG_SHADING_RENDERER_HPP__

// Phong 着色渲染器头文件
// 提供基础的光线投射渲染流程和阴影判断

#include "scene/Scene.hpp"
#include "Camera.hpp"
#include "intersections/intersections.hpp"
#include "shaders/ShaderCreator.hpp"
#include <tuple>
#include <array>
#include <unordered_map>

namespace PhongShading
{
    using namespace NRenderer;

    // Phong 着色渲染器
    class PhongShadingRenderer
    {
    private:
        SharedScene spScene;                    // 场景指针
        Scene& scene;                           // 场景引用
        PhongShading::Camera camera;            // 相机
        vector<SharedShader> shaderPrograms;    // 着色器程序列表

        // 三角形顶点法线（插值用）；每个三角形对应 3 个顶点法线
        vector<std::array<Vec3, 3>> triVertexNormals;
        // 最近一次三角形命中的索引（用于在 trace 阶段进行法线插值）
        int lastHitTriangleIndex = -1;

    public:
        // 构造函数
        PhongShadingRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {}

        ~PhongShadingRenderer() = default;

        // 渲染结果类型：像素数组指针 + 宽 + 高
        using RenderResult = std::tuple<RGBA*, unsigned int, unsigned int>;

        // 执行渲染
        RenderResult render();

        // 释放渲染结果像素数组内存
        void release(const RenderResult& r);

        // Gamma 校正（开平方）
        RGB gamma(const RGB& rgb);

        // 计算三角形顶点法线（位置量化 + 相邻累加 + 归一化）
        void computeTriangleVertexNormals();

    private:
        // 光线追踪，返回该光线的颜色
        RGB trace(const Ray& r);

        // 计算最近的几何体命中记录
        HitRecord closestHit(const Ray& r);
    };
}

#endif
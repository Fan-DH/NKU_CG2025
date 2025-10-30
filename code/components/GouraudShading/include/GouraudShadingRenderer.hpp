#pragma once
#ifndef __GOURAUD_SHADING_RENDERER_HPP__
#define __GOURAUD_SHADING_RENDERER_HPP__

// Gouraud 着色渲染器头文件
// 预计算三角形顶点颜色并在像素级进行重心插值

#include "scene/Scene.hpp"
#include "Camera.hpp"
#include "intersections/intersections.hpp"
#include "shaders/ShaderCreator.hpp"
#include <tuple>
#include <array>
#include <unordered_map>

namespace GouraudShading
{
    using namespace NRenderer;

    // Gouraud 着色渲染器
    class GouraudShadingRenderer
    {
    private:
        SharedScene spScene;                    // 场景指针
        Scene& scene;                           // 场景引用
        GouraudShading::Camera camera;          // 相机
        vector<SharedShader> shaderPrograms;    // 着色器程序列表

        // 顶点法线（每个三角形对应 3 个顶点法线）
        vector<std::array<Vec3, 3>> triVertexNormals;
        // 顶点颜色（每个三角形对应 3 个顶点颜色）
        vector<std::array<RGB, 3>> triVertexColors;
        // 最近一次三角形命中的索引（用于插值）
        int lastHitTriangleIndex = -1;

    public:
        // 构造函数
        GouraudShadingRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {}

        ~GouraudShadingRenderer() = default;

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

        // 预计算每个三角形的三个顶点颜色（使用着色器）
        void precomputeTriangleVertexColors();

    private:
        // 光线追踪，返回该光线的颜色
        RGB trace(const Ray& r);

        // 计算最近的几何体命中记录
        HitRecord closestHit(const Ray& r);
    };
}

#endif
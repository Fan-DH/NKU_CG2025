#pragma once
#ifndef __COOK_TORRANCE_HPP__
#define __COOK_TORRANCE_HPP__

// Cook-Torrance 材质着色器头文件
// 实现了基于微表面模型的 PBR 着色（GGX/Smith/Schlick）

#include "Shader.hpp"

namespace RayCast
{
    // Cook-Torrance 材质着色器类
    // 组成：D(法线分布-GGX)、G(几何遮蔽-Smith SchlickGGX)、F(菲涅尔-Schlick)
    class CookTorrance : public Shader
    {
    private:
        RGB albedo;       // 基础颜色（BaseColor/Albedo）
        float roughness;  // 粗糙度 [0,1]
        float metallic;   // 金属度 [0,1]
        RGB F0;           // 法向入射反射率（可选，未提供时按标准混合计算）

    public:
        // 构造函数
        // material: 材质参数
        // textures: 纹理数组
        CookTorrance(Material& material, vector<Texture>& textures);

        // 计算着色结果
        // in: 视线方向
        // out: 光源方向
        // normal: 表面法线
        // 返回着色计算的颜色值
        virtual RGB shade(const Vec3& in, const Vec3& out, const Vec3& normal) const override;
    };
}

#endif
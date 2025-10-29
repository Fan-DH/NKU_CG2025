// Cook-Torrance 材质着色器实现
// 采用 GGX 法线分布、Smith-Schlick 几何项与 Schlick 菲涅尔近似
#include <algorithm>
#include "shaders/CookTorrance.hpp"

namespace RayCast
{
    using PW = Property::Wrapper;

    // GGX / Trowbridge-Reitz 法线分布函数 D
    static inline float D_GGX(float NdotH, float alpha)
    {
        // alpha 为粗糙度平方，常见实现使用 a2 = alpha^2
        float a2 = alpha * alpha;
        float denom = (NdotH * NdotH) * (a2 - 1.0f) + 1.0f;
        return a2 / (PI * denom * denom);
    }

    // Smith-Schlick 几何遮蔽函数 G1（单边）
    static inline float G_SchlickGGX(float NdotX, float alpha)
    {
        // 若传入的 alpha 为粗糙度平方，则 k = alpha / 2
        float k = alpha * 0.5f;
        return NdotX / (NdotX * (1.0f - k) + k);
    }

    // Schlick 菲涅尔近似 F
    static inline RGB F_Schlick(float VdotH, const RGB& F0)
    {
        return F0 + (RGB{1.0f} - F0) * glm::pow(1.0f - VdotH, 5.0f);
    }

    // 构造函数：读取 Cook-Torrance 所需材质参数
    CookTorrance::CookTorrance(Material& material, vector<Texture>& textures)
        : Shader(material, textures)
        , albedo{1.0f, 1.0f, 1.0f}
        , roughness{0.5f}
        , metallic{0.0f}
        , F0{0.04f, 0.04f, 0.04f}
    {
        // 优先读取 albedo；若不存在则回退到 diffuseColor
        if (auto optAlbedo = material.getProperty<PW::RGBType>("albedo")) {
            albedo = (*optAlbedo).value;
        } else if (auto optDiffuseColor = material.getProperty<PW::RGBType>("diffuseColor")) {
            albedo = (*optDiffuseColor).value;
        }

        if (auto optRoughness = material.getProperty<PW::FloatType>("roughness")) {
            roughness = (*optRoughness).value;
        }
        if (auto optMetallic = material.getProperty<PW::FloatType>("metallic")) {
            metallic = (*optMetallic).value;
        }
        if (auto optF0 = material.getProperty<PW::RGBType>("F0")) {
            F0 = (*optF0).value;
        }

        // 简单约束，避免数值异常
        roughness = glm::clamp(roughness, 0.0f, 1.0f);
        metallic  = glm::clamp(metallic,  0.0f, 1.0f);
        F0 = NRenderer::clamp(F0);
        albedo = NRenderer::clamp(albedo);
    }

    // 计算着色结果
    // in: 视线方向 V
    // out: 光源方向 L
    // normal: 表面法线 N
    // 返回 Lo = (diffuse + spec) * NdotL
    RGB CookTorrance::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const
    {
        Vec3 N = glm::normalize(normal);
        Vec3 V = glm::normalize(in);
        Vec3 L = glm::normalize(out);

        Vec3 H = glm::normalize(V + L);

        float NdotL = glm::max(glm::dot(N, L), 0.0f);
        float NdotV = glm::max(glm::dot(N, V), 0.0f);
        float NdotH = glm::max(glm::dot(N, H), 0.0f);
        float VdotH = glm::max(glm::dot(V, H), 0.0f);

        // alpha 为粗糙度平方（避免 0 带来的除零问题）
        float alpha = roughness * roughness;

        // 法线分布 D
        float D = D_GGX(NdotH, alpha);

        // 几何遮蔽 G（入射与出射）
        float Gv = G_SchlickGGX(NdotV, alpha);
        float Gl = G_SchlickGGX(NdotL, alpha);
        float G  = Gv * Gl;

        // 法向入射反射率 F0：若材质未提供，则按标准混合
        RGB F0_used = F0;
        if (!material.hasProperty("F0")) {
            // 常用做法：F0 = mix(0.04, albedo, metallic)
            F0_used = glm::mix(RGB{0.04f, 0.04f, 0.04f}, albedo, metallic);
        }
        RGB F = F_Schlick(VdotH, F0_used);

        // 镜面反射分量（加入 max 以避免除零）
        float denom = std::max(1e-6f, 4.0f * NdotL * NdotV);
        RGB spec = (D * G / denom) * F;

        // 能量守恒：kS = F，非金属才分配漫反射能量 kD
        RGB kS = F;
        RGB kD = (RGB{1.0f} - kS) * (1.0f - metallic);

        // 漫反射（Lambert / PI）
        RGB diffuse = (albedo / PI) * kD;

        // 出射辐射 Lo
        return (diffuse + spec) * NdotL;
    }
}
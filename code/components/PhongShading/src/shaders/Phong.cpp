// Phong������ɫ��ʵ��
// ʵ����Phong����ģ�͵���ɫ����
#include "shaders/Phong.hpp"

namespace PhongShading
{
    // ���㷴������
    // normal: ���淨��
    // dir: ���䷽��
    // ���ط��䷽��
    Vec3 reflect(const Vec3& normal, const Vec3& dir) {
        return dir - 2*glm::dot(dir, normal)*normal;
    }

    // ���캯��
    // material: ���ʲ���
    // textures: ��������
    Phong::Phong(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        using PW = Property::Wrapper;
        // ��ȡ��������ɫ
        auto optDiffuseColor = material.getProperty<PW::RGBType>("diffuseColor");
        if (optDiffuseColor) diffuseColor = (*optDiffuseColor).value;
        else diffuseColor = {1, 1, 1};  // Ĭ��Ϊ��ɫ
        
        // ��ȡ���淴����ɫ
        auto optSpecularColor = material.getProperty<PW::RGBType>("specularColor");
        if (optSpecularColor) specularColor = (*optSpecularColor).value;
        else specularColor = {1, 1, 1};  // Ĭ��Ϊ��ɫ

        // ��ȡ���淴��ָ��
        auto optSpecularEx = material.getProperty<PW::FloatType>("specularEx");
        if (optSpecularEx) specularEx = (*optSpecularEx).value;
        else specularEx = 1;  // Ĭ��Ϊ1
    }

    // ������ɫ���
    // in: ���߷���
    // out: ��Դ����
    // normal: ���淨��
    // ������ɫ�������ɫֵ
    RGB Phong::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const {
        Vec3 v = in;  // ���߷���
        Vec3 r = reflect(normal, out);  // ���䷽��
        // �����������diffuseColor * cos(theta)
        auto diffuse = diffuseColor * glm::dot(out, normal);
        // ���㾵�淴���specularColor * (cos(alpha))^n
        auto specular = specularColor * fabs(glm::pow(glm::dot(v, r), specularEx));
        // ����������;��淴��ĵ���
        return diffuse + specular;
    }
}
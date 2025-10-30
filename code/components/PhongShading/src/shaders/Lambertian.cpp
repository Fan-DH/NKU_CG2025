// Lambert������ɫ��ʵ��
// ʵ����Lambert��������ʵ���ɫ����
#include "shaders/Lambertian.hpp"

namespace PhongShading
{
    // ���캯��
    // material: ���ʲ���
    // textures: ��������
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        // ��ȡ��������ɫ
        auto optDiffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (optDiffuseColor) diffuseColor = (*optDiffuseColor).value;
        else diffuseColor = {1, 1, 1};  // Ĭ��Ϊ��ɫ
    }

    // ������ɫ���
    // in: ���䷽��
    // out: ���䷽��
    // normal: ���淨��
    // ������ɫ�������ɫֵ
    RGB Lambertian::shade(const Vec3& in, const Vec3& out, const Vec3& normal) const {
        // ����Lambert�����䣺diffuseColor * cos(theta)
        return diffuseColor * glm::dot(out, normal);
    }
}
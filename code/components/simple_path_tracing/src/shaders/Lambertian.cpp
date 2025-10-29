#include "shaders/Lambertian.hpp"
#include "samplers/SamplerInstance.hpp"

#include "Onb.hpp"

namespace SimplePathTracer
{
    /**
     * Lambertian��ɫ�����캯��
     * @param material ���ʶ���
     * @param textures ��������
     */
    Lambertian::Lambertian(Material& material, vector<Texture>& textures)
        : Shader                (material, textures)
    {
        // ��ȡ���ʵ���������ɫ
        auto diffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        if (diffuseColor) albedo = (*diffuseColor).value;
        else albedo = {1, 1, 1};  // Ĭ�ϰ�ɫ
    }
    
    /**
     * Lambertianɢ�����
     * ʵ������������䣬���������з����Ͼ���ɢ��
     * @param ray �������
     * @param hitPoint �ཻ��
     * @param normal ������
     * @return ɢ����Ϣ
     */
    Scattered Lambertian::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const {
        Vec3 origin = hitPoint;
        
        // �ڰ����ھ��Ȳ���ɢ�䷽��
        Vec3 random = defaultSamplerInstance<HemiSphere>().sample3d();
        
        // ʹ��������������������ת�����Է�����Ϊz��ľֲ�����ϵ
        Onb onb{normal};
        Vec3 direction = glm::normalize(onb.local(random));

        // Lambertianɢ��ĸ����ܶȺ�����1/(2��)
        float pdf = 1/(2*PI);

        // Lambertian BRDF��albedo/��
        auto attenuation = albedo / PI;

        return {
            Ray{origin, direction},  // ɢ�����
            attenuation,             // ˥��ϵ��
            Vec3{0},                // ���Է���
            pdf                     // �����ܶȺ���ֵ
        };
    }
}
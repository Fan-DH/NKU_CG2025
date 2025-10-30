// �ཻ���Ժ���ʵ��
// ʵ���˸��ּ���������ߵ��ཻ�����㷨
#include "intersections/intersections.hpp"

namespace PhongShading::Intersection
{
    // �����������ε��ཻ?ller�CTrumbore�㷨��������������ε��ཻ
    // ray: ����
    // t: ������
    // tMin, tMax: �ཻ���뷶Χ
    HitRecord xTriangle(const Ray& ray, const Triangle& t, float tMin, float tMax) {
        const auto& v1 = t.v1;
        const auto& v2 = t.v2;
        const auto& v3 = t.v3;
        const auto& normal = glm::normalize(t.normal);
        // ���������ε�����������
        auto e1 = v2 - v1;
        auto e2 = v3 - v1;
        // ������P
        auto P = glm::cross(ray.direction, e2);
        float det = glm::dot(e1, P);
        // ���������޳�
        Vec3 T;
        if (det > 0) T = ray.origin - v1;
        else { T = v1 - ray.origin; det = -det; }
        // �ж��Ƿ�ƽ��
        if (det < 0.000001f) return getMissRecord();
        // ������������
        float u, v, w;
        u = glm::dot(T, P);
        if (u > det || u < 0.f) return getMissRecord();
        Vec3 Q = glm::cross(T, e1);
        v = glm::dot(ray.direction, Q);
        if (v < 0.f || v + u > det) return getMissRecord();
        w = glm::dot(e2, Q);
        float invDet = 1.f / det;
        w *= invDet;
        // ����ཻ�����Ƿ�����Ч��Χ��
        if (w >= tMax || w <= tMin) return getMissRecord();
        return getHitRecord(w, ray.at(w), normal, t.material);
    }

    // ������������ཻ����
    // ʹ�ý������η������������������ཻ
    // ray: ����
    // s: ����
    // tMin, tMax: �ཻ���뷶Χ
    HitRecord xSphere(const Ray& ray, const Sphere& s, float tMin, float tMax) {
        const auto& position = s.position;
        const auto& r = s.radius;
        // ������η���ϵ��
        Vec3 oc = ray.origin - position;
        float a = glm::dot(ray.direction, ray.direction);
        float b = glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - r*r;
        // �����б�ʽ
        float discriminant = b*b - a*c;
        float sqrtDiscriminant = sqrt(discriminant);
        // �����ʵ��
        if (discriminant > 0) {
            // ���ԽϽ��Ľ���
            float temp = (-b - sqrtDiscriminant) / a;
            if (temp < tMax && temp > tMin) {
                auto hitPoint = ray.at(temp);
                auto normal = (hitPoint - position)/r;
                return getHitRecord(temp, hitPoint, normal, s.material);
            }
            // ���Խ�Զ�Ľ���
            temp = (-b + sqrtDiscriminant) / a;
            if (temp < tMax && temp > tMin) {
                auto hitPoint = ray.at(temp);
                auto normal = (hitPoint - position)/r;
                return getHitRecord(temp, hitPoint, normal, s.material);
            }
        }
        return getMissRecord();
    }

    // ������ƽ����ཻ����
    // ray: ����
    // p: ƽ��
    // tMin, tMax: �ཻ���뷶Χ
    HitRecord xPlane(const Ray& ray, const Plane& p, float tMin, float tMax) {
        // ����ƽ�淨��
        Vec3 normal = glm::normalize(p.normal);
        // ������߷�����ƽ�淨�ߵĵ��
        auto Np_dot_d = glm::dot(ray.direction, normal);
        // �ж��Ƿ�ƽ��
        if (Np_dot_d < 0.0000001f && Np_dot_d > -0.00000001f) return getMissRecord();
        // �����ཻ����
        float dp = -glm::dot(p.position, normal);
        float t = (-dp - glm::dot(normal, ray.origin))/Np_dot_d;
        // ����ཻ�����Ƿ�����Ч��Χ��
        if (t >= tMax || t <= tMin) return getMissRecord();
        // �����ཻ��
        Vec3 hitPoint = ray.at(t);
        // ����ཻ���Ƿ���ƽ�淶Χ��
        Mat3x3 d{p.u, p.v, glm::cross(p.u, p.v)};
        d = glm::inverse(d);
        auto res  = d * (hitPoint - p.position);
        auto u = res.x, v = res.y;
        if ((u<=1 && u>=0) && (v<=1 && v>=0)) {
            return getHitRecord(t, hitPoint, normal, p.material);
        }
        return getMissRecord();
    }

    // ���������Դ���ཻ����
    // ray: ����
    // a: ���Դ
    // tMin, tMax: �ཻ���뷶Χ
    HitRecord xAreaLight(const Ray& ray, const AreaLight& a, float tMin, float tMax) {
        // �����Դƽ��ķ���
        Vec3 normal = glm::cross(a.u, a.v);
        Vec3 position = a.position;
        // ������߷�����ƽ�淨�ߵĵ��
        auto Np_dot_d = glm::dot(ray.direction, normal);
        // �ж��Ƿ�ƽ��
        if (Np_dot_d < 0.0000001f && Np_dot_d > -0.00000001f) return getMissRecord();
        // �����ཻ����
        float dp = -glm::dot(position, normal);
        float t = (-dp - glm::dot(normal, ray.origin))/Np_dot_d;
        // ����ཻ�����Ƿ�����Ч��Χ��
        if (t >= tMax || t <= tMin) return getMissRecord();
        // �����ཻ��
        Vec3 hitPoint = ray.at(t);
        // ����ཻ���Ƿ��ڹ�Դ��Χ��
        Mat3x3 d{a.u, a.v, glm::cross(a.u, a.v)};
        d = glm::inverse(d);
        auto res  = d * (hitPoint - position);
        auto u = res.x, v = res.y;
        if ((u<=1 && u>=0) && (v<=1 && v>=0)) {
            return getHitRecord(t, hitPoint, normal, {});
        }
        return getMissRecord();
    }
}
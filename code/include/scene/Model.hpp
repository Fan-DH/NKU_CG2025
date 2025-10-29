// ģ��ϵͳ����
// �����˳����еĸ��ּ���ʵ���ģ�ͽṹ
#pragma once
#ifndef __NR_MODEL_HPP__
#define __NR_MODEL_HPP__

#include <string>
#include <vector>

#include "geometry/vec.hpp"

#include "Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    using namespace std;

    // ʵ�����
    // ���м���ʵ��Ļ��࣬����������Ϣ
    struct Entity {
        Handle material;    // ���ʾ��
    };
    SHARE(Entity);

    // ����ʵ��
    // ������һ�������λ�á�����Ͱ뾶
    struct Sphere : public Entity
    {
        Vec3 direction = {0, 0, 1};  // ���巽��
        Vec3 position = {0, 0, 0};   // ����λ��
        float radius = { 0 };        // ����뾶
    };
    SHARE(Sphere);
    
    // ������ʵ��
    // ������һ�������ε���������ͷ���
    struct Triangle : public Entity
    {
        union {
            struct {
                Vec3 v1;    // ��һ������
                Vec3 v2;    // �ڶ�������
                Vec3 v3;    // ����������
            };
            Vec3 v[3];     // ����������ʽ
        };
        Vec3 normal;       // �����η���

        // Ĭ�Ϲ��캯��
        Triangle()
            : v1            ()
            , v2            ()
            , v3            ()
            , normal         (0, 0, 1)
        {}
    };
    SHARE(Triangle);

    // ƽ��ʵ��
    // ������һ��ƽ���λ�á����ߺ�����������
    struct Plane : public Entity
    {
        Vec3 normal = {0, 0, 1};  // ƽ�淨��
        Vec3 position = {};       // ƽ���ϵ�һ��
        Vec3 u = {};             // ƽ���һ��������
        Vec3 v = {};             // ƽ��ڶ���������
    };
    SHARE(Plane);

    // ����ʵ��
    // ������һ���ɶ����������ɵ�����
    struct Mesh : public Entity
    {
        vector<Vec3> normals;         // �����б�
        vector<Vec3> positions;       // ����λ���б�
        vector<Vec2> uvs;             // UV�����б�
        vector<Index> normalIndices;   // ��������
        vector<Index> positionIndices; // ��������
        vector<Index> uvIndices;       // UV����

        // ����Ƿ��з�������
        bool hasNormal() const {
            return normals.size() != 0;
        }

        // ����Ƿ���UV��������
        bool hasUv() const {
            return uvs.size() != 0;
        }
    };
    SHARE(Mesh);

    // �����ڵ�
    // ��ʾ����ͼ�е�һ���ڵ㣬�����ǲ�ͬ���͵ļ���ʵ��
    struct Node
    {
        // �ڵ�����ö��
        enum class Type
        {
            SPHERE = 0x0,    // ����
            TRIANGLE = 0X1,   // ������
            PLANE = 0X2,      // ƽ��
            MESH = 0X3        // ����
        };
        Type type = Type::SPHERE;  // �ڵ�����
        Index entity;              // ʵ������
        Index model;               // ����ģ������
    };
    SHARE(Node);

    // ģ�ͽṹ
    // ������һ��������3Dģ�ͣ���������ڵ�ͱ任��Ϣ
    struct Model {
        vector<Index> nodes;           // �ڵ��б�
        Vec3 translation = {0, 0, 0};  // ƽ������
        Vec3 scale = {1, 1, 1};       // ��������
    };
    SHARE(Model);
}

#endif
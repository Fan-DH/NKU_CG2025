// ����ϵͳ����
// �ṩ�����Ĳ������Թ���ϵͳ��֧�ֶ������͵Ĳ�������
#pragma once

#ifndef __NR_MATERIAL_HPP__
#define __NR_MATERIAL_HPP__

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "geometry/vec.hpp"
#include "Texture.hpp"

#include <optional>
#include <algorithm>

namespace NRenderer
{
    using namespace std;

    // �������Խṹ��
    // �����˲��ʵĸ����������ͺ�ֵ�ķ�װ
    struct Property
    {
        // ����ֵ��װ����
        // Ϊ��ͬ���͵�����ֵ�ṩͳһ�Ľӿ�
        class Wrapper
        {
        private:
            template<typename T>
            struct Base { T value = {}; };
        public:
            struct IntType : public Base<int> {};         // ��������
            struct FloatType : public Base<float>{};      // ����������
            struct RGBType : public Base<RGB> {};         // RGB��ɫ����
            struct RGBAType: public Base<RGBA> {};        // RGBA��ɫ����
            struct Vec3Type : public Base<Vec3> {};       // 3D��������
            struct Vec4Type : public Base<Vec4> {};       // 4D��������
            struct TextureIdType : public Base<Handle> {}; // ����ID����
        };

        // ��������ö��
        enum class Type
        {
            INT = 0x0,      // ��������
            FLOAT,          // ����������
            RGB,            // RGB��ɫ
            RGBA,           // RGBA��ɫ
            VEC3,           // 3D����
            VEC4,           // 4D����
            TEXTURE_ID      // ����ID
        };

    #define Types Wrapper::IntType, Wrapper::FloatType, Wrapper::RGBType, Wrapper::RGBAType, Wrapper::Vec3Type, Wrapper::Vec4Type, Wrapper::TextureIdType
        using ValueType = variant<Types>;  // ʹ��variant�洢��ͬ���͵�ֵ
    #undef Types

        string key;         // ��������
        Type type;         // ��������
        ValueType valueWrapper;  // ����ֵ

        // �����������ԵĹ��캯��
        Property(const string& key, Wrapper::IntType intWrapper)
            : key               (key)
            , type              (Type::INT)
            , valueWrapper      (intWrapper)
        {}
        Property(const string& key, Wrapper::FloatType floatWrapper)
            : key               (key)
            , type              (Type::FLOAT)
            , valueWrapper      (floatWrapper)
        {}
        Property(const string& key, Wrapper::RGBType rgbWrapper)
            : key               (key)
            , type              (Type::RGB)
            , valueWrapper      (rgbWrapper)
        {}
        Property(const string& key, Wrapper::RGBAType rgbaWrapper)
            : key               (key)
            , type              (Type::RGBA)
            , valueWrapper      (rgbaWrapper)
        {}
        Property(const string& key, Wrapper::Vec3Type vec3Wrapper)
            : key               (key)
            , type              (Type::VEC3)
            , valueWrapper      (vec3Wrapper)
        {}
        Property(const string& key, Wrapper::Vec4Type vec4Wrapper)
            : key               (key)
            , type              (Type::VEC4)
            , valueWrapper      (vec4Wrapper)
        {}
        Property(const string& key, Wrapper::TextureIdType texIdWrapper)
            : key               (key)
            , type              (Type::TEXTURE_ID)
            , valueWrapper      (texIdWrapper)
        {}
    };

    // ���ʽṹ��
    // ����һ��������ԣ��ṩ���Ե���ӡ�ɾ���Ͳ�ѯ����
    struct Material
    {
        Material() = default;

    public:
        unsigned int type = 0;           // ��������
        vector<Property> properties;     // ���������б�

        // ����Ƿ����ָ������������
        inline
        bool hasProperty(const string& key) {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            return it != properties.end();
        }

        // ��ȡָ������������ֵ
        // ����optional���ͣ�������Բ������򷵻�nullopt
        template<typename T>
        auto getProperty(const string& key) -> optional<T> {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            if (it == properties.end()) return nullopt;
            else {
                auto prop = *it;
                auto v = std::get<T>(prop.valueWrapper);
                return optional<T>(v);
            }
        }

        // ע���µ�����
        // ��������Ѵ����򷵻�false
        template<typename T>
        bool registerProperty(const string& key, T value) {
            using PropType = Property::Type;
            bool fail = hasProperty(key);
            if (!fail) {
                properties.push_back(Property{key, value});
            }
            return !fail;
        }

        // ע���ѹ�������Զ���
        bool registerProperty(const Property& prop) {
            bool fail = hasProperty(prop.key);
            if (!fail) {
                properties.push_back(prop);
            }
            return !fail;
        }

        // ��ȡ��������
        auto getPropertiesNum() const {
            return properties.size();
        }

        // ���ݼ���ɾ������
        void removeProperty(const string& key) {
            auto it = find_if(properties.begin(),
                properties.end(),
                [&key](const Property& p1) -> bool {
                    return p1.key == key;
                }
            );
            if (it != properties.end()) {
                properties.erase(it);
            }
        }
        
        // ��������ɾ������
        void removeProperty(Index i) {
            if (i < properties.size()) {
                properties.erase(properties.begin() + i);
            }
        }
    };
    using SharedMaterial = shared_ptr<Material>;  // ���ʵ�����ָ������
}

#endif
#ifndef __NR_MATERIAL_TEMPLATES_HPP__
#define __NR_MATERIAL_TEMPLATES_HPP__

// ����ģ��ͷ�ļ�
// ������Ԥ�����ģ��Ĺ�����

#include <unordered_map>

#include "scene/Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    // ����ģ����
    // ������ṩԤ��Ĳ���ģ��
    class DLL_EXPORT MaterialTemplates
    {
    public:
        // ģ��ṹ��
        // ���嵥������ģ�������
        struct Template
        {
            unsigned int index;      // ģ������
            string key;             // ģ������
            vector<Property> props;  // ���������б�
        };

        static unordered_map<int, Template> templates;  // ����ģ��ӳ���

    private:
        // ģ���ʼ���ṹ��
        // ���𴴽�������Ԥ�����ģ��
        struct InitTemplates
        {
            // ��ӵ������Ե�ģ��
            // p: Ŀ��ģ��
            // arg: Ҫ��ӵ�����
            template<typename T>
            void PushProp(Template& p, T&& arg) {
                p.props.push_back(arg);
            }

            // ��Ӷ�����Ե�ģ��
            // p: Ŀ��ģ��
            // arg1: ��һ������
            // args: ��������
            template<typename T, typename ...Args>
            void PushProp(Template& p, T&& arg1, Args&& ...args) {
                p.props.push_back(arg1);
                PushProp(p, args...);
            }

            // ����µĲ���ģ��
            // templates: ģ��ӳ���
            // index: ģ������
            // name: ģ������
            // args: ģ�������б�
            template<typename ...Args>
            void Add(unordered_map<int, Template>& templates, unsigned int index, const string& name, Args&& ...args) {
                templates[index] = {
                    index, name, {}
                };
                PushProp(templates[index], args...);
            }

            // ���캯��
            // ��ʼ��Ԥ�����ģ��
            InitTemplates(unordered_map<int, Template>& templates) {
                using PW = Property::Wrapper;
                // Lambertian���������
                Add(templates, 0, string("Lambertian"),
                    Property("diffuseColor", PW::RGBType{}),      // ��������ɫ
                    Property("diffuseMap", PW::TextureIdType{})   // ��������ͼ
                );
                // Phong����
                Add(templates, 1, string("Phong"),
                    Property("diffuseColor", PW::RGBType{}),      // ��������ɫ
                    Property("diffuseMap", PW::TextureIdType{}),  // ��������ͼ
                    Property("specularColor", PW::RGBType{}),     // �߹���ɫ
                    Property("specularEx", PW::FloatType{})       // �߹�ָ��
                );
                // ����ʲ���
                Add(templates, 2, string("Dielectric"),
                    Property("ior", PW::FloatType{float(1.0)}),   // ������
                    Property("absorbed", PW::RGBType{RGB{1, 1, 1}})  // ������
                );
                // �������
                Add(templates, 3, string("Conductor"),
                    Property("reflect", PW::RGBType{RGB{1, 1, 1}})  // ������
                );
                // ���ϲ���
                Add(templates, 4, string("Plastic"),
                    Property("diffuseColor", PW::RGBType{}),      // ��������ɫ
                    Property("specularColor", PW::RGBType{}),     // �߹���ɫ
                    Property("refractIndex", PW::FloatType{})     // ������
                );
                
            }
        };
        static InitTemplates initTemplates;  // ģ���ʼ����ʵ��
    };
}

#endif
// �����ļ�������ʵ���ļ�
// �������Զ���SCN��ʽ�ĳ����ļ����������ʡ�ģ�ͺ͹�Դ�Ķ���

#include "importer/ScnImporter.hpp"

#include <fstream>
#include <sstream>

#include <map>

namespace NRenderer
{
    // �������ʶ��岿��
    // asset: �ʲ�������
    // file: �����ļ�������
    // mtlMap: �������Ƶ�������ӳ��
    // ����ֵ: �����Ƿ�ɹ�
    bool ScnImporter::parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        SharedMaterial spMaterial{nullptr};  // ��ǰ����Ĳ���

        bool successFlag = true;
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token=="") continue;
            else if (token[0] == '#') continue;  // ����ע����
            else if (token == "Material") {  // ���ʶ��忪ʼ
                // ��ȡ��������
                string name;
                ss>>name;
                MaterialItem item;
                spMaterial = SharedMaterial{new Material()};
                item.material = spMaterial;
                item.name = name;
                auto index = asset.materialItems.size();
                asset.materialItems.push_back(item);
                if (mtlMap.find(name) == mtlMap.end()) {
                    mtlMap[name] = index;
                }
                else {
                    lastErrorInfo = "Duplicated Material Key:" + name;
                    successFlag = false;
                    break;
                }
                // ��ȡ��������
                unsigned int type = 0;
                if (!ss.eof())
                    ss>>type;
                spMaterial->type = type;
            }
            else if (token == "Prop") {  // �������Զ���
                using PT = Property::Type;
                using PW = Property::Wrapper;
                string key, type;
                ss>>key>>type;
                // �����������ͽ�����ͬ��ʽ������
                if (type == "Int") {  // ��������
                    int v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::IntType{v});
                }
                else if (type == "Float") {  // ����������
                    float v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::FloatType{v});
                }
                else if (type == "Vec3") {  // ��ά��������
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::Vec3Type{Vec3{v1, v2, v3}});
                }
                else if (type == "Vec4") {  // ��ά��������
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::Vec4Type{Vec4{v1, v2, v3, v4}});
                }
                else if (type == "RGB") {  // RGB��ɫ����
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::RGBType{Vec3{v1, v2, v3}});
                }
                else if (type == "RGBA") {  // RGBA��ɫ����
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::RGBAType{Vec4{v1, v2, v3, v4}});
                }
            }
            else if (token == "End") {  // ���ʶ������
                break;
            }
            else {
                successFlag = false;
                break;
            }
        }

        return successFlag;
    }

    // ����ģ�Ͷ��岿��
    // asset: �ʲ�������
    // file: �����ļ�������
    // mtlMap: �������Ƶ�������ӳ��
    // ����ֵ: �����Ƿ�ɹ�
    bool ScnImporter::parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        
        ModelItem modelItem;

        bool successFlag = true;

        int currNodeType = 0;  // ��ǰ�ڵ�����
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token=="") continue;
            else if (token[0] == '#') continue;  // ����ע����
            else if (token == "Model") {  // ģ�Ͷ��忪ʼ
                modelItem = ModelItem{};
                ss>>modelItem.name;
                modelItem.model = make_shared<Model>();
                asset.modelItems.push_back(modelItem);
            }
            else if (token == "Translation") {  // ģ��ƽ��
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->translation = {f1, f2, f3};
            }
            else if (token == "Scale") {  // ģ������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->scale = {f1, f2, f3};
            }
            else if (token == "Sphere") {  // ����ڵ�
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::SPHERE;
                currNodeType = 0;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.spheres.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.spheres.push_back(SharedSphere{new Sphere()});
                asset.spheres[asset.spheres.size() - 1]->material = mtl->second;
            }
            else if (token == "Triangle") {  // �����νڵ�
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::TRIANGLE;
                currNodeType = 1;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                ni.node->entity = asset.triangles.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                asset.nodeItems.push_back(ni);
                asset.triangles.push_back(SharedTriangle{new Triangle()});
                asset.triangles[asset.triangles.size() - 1]->material = mtl->second;
            }
            else if (token == "Plane") {  // ƽ��ڵ�
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::PLANE;
                currNodeType = 2;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.planes.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.planes.push_back(SharedPlane{new Plane()});
                asset.planes[asset.planes.size() - 1]->material = mtl->second;
            }
            else if (token == "R") {  // ����뾶
                float f;
                ss>>f;
                auto it = asset.spheres.end()-1;
                (*it)->radius = f;
            }
            else if (token == "N") {  // ���߷���
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 n = {f1, f2, f3};
                if (currNodeType == 0) {  // ���巨��
                    auto it = asset.spheres.end() - 1;
                    (*it)->direction = n;
                }
                else if (currNodeType == 1) {  // �����η���
                    auto it = asset.triangles.end() - 1;
                    (*it)->normal = n;
                } 
                else if (currNodeType == 2) {  // ƽ�淨��
                    auto it = asset.planes.end() - 1;
                    (*it)->normal = n;
                }
            }
            else if (token == "V1") {  // �����ε�һ������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[0] = v;
            }
            else if (token == "V2") {  // �����εڶ�������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[1] = v;
            }
            else if (token == "V3") {  // �����ε���������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[2] = v;
            }
            else if (token == "P") {  // λ��
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 p = {f1, f2, f3};
                if (currNodeType == 0) {  // ����λ��
                    auto it = asset.spheres.end() - 1;
                    (*it)->position = p;
                }
                else if (currNodeType == 2) {  // ƽ��λ��
                    auto it = asset.planes.end() - 1;
                    (*it)->position = p;
                }
            }
            else if (token == "U") {  // ƽ��U��������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->u = u;
            }
            else if (token == "V") {  // ƽ��V��������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->v = v;
            }
            else if (token == "End") {  // ģ�Ͷ������
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    // ������Դ���岿��
    // asset: �ʲ�������
    // file: �����ļ�������
    // ����ֵ: �����Ƿ�ɹ�
    bool ScnImporter::parseLgt(Asset& asset, ifstream& file) {
        bool successFlag = true;

        string currline;
        stringstream ss{};

        int currLightType{0};  // ��ǰ��Դ����
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token == "") continue;

            else if (token[0] == '#') continue;  // ����ע����

            else if (token == "Point") {  // ���Դ
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 0;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::POINT;
                li.light->entity = asset.pointLights.size();
                asset.lightItems.push_back(li);
                asset.pointLights.push_back(make_shared<PointLight>());
            }
            else if (token == "Spot") {  // �۹��
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 3;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::SPOT;
                li.light->entity = asset.spotLights.size();
                asset.lightItems.push_back(li);
                asset.spotLights.push_back(make_shared<SpotLight>());
            }
            else if (token == "Directional") {  // ƽ�й�
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 2;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::DIRECTIONAL;
                li.light->entity = asset.directionalLights.size();
                asset.lightItems.push_back(li);
                asset.directionalLights.push_back(make_shared<DirectionalLight>());
            }
            else if (token == "Area") {  // ���Դ
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 1;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::AREA;
                li.light->entity = asset.areaLights.size();
                asset.lightItems.push_back(li);
                asset.areaLights.push_back(make_shared<AreaLight>());
            }
            else if (token == "IRV") {  // ��Դǿ��/�����/���ն�
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                if (currLightType == 0) {  // ���Դǿ��
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.intensity = v;
                }
                else if (currLightType == 1) {  // ���Դ�����
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.radiance = v;
                }
                else if (currLightType == 2) {  // ƽ�й���ն�
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.irradiance = v;
                }
                else if (currLightType == 3) {  // �۹��ǿ��
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.intensity = v;
                }
            }
            else if (token == "P") {  // ��Դλ��
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 pos{f1, f2, f3};
                if (currLightType == 0) {  // ���Դλ��
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.position = pos;
                }
                else if (currLightType == 1) {  // ���Դλ��
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.position = pos;
                }
                else if (currLightType == 3) {  // �۹��λ��
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.position = pos;
                }
            }
            else if (token == "D") {  // ��Դ����
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 dir{f1, f2, f3};
                if (currLightType == 2) {  // ƽ�йⷽ��
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.direction = dir;
                }
                else if (currLightType == 3) {  // �۹�Ʒ���
                    auto& s = *(*(asset.directionalLights.end() - 1));
                    s.direction = dir;
                }
            }
            else if (token == "HotSpot") {  // �۹���ȵ�Ƕ�
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.hotSpot = r;
            }
            else if (token == "Fallout") {  // �۹��˥���Ƕ�
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.fallout = r;
            }
            else if (token == "U") {  // ���ԴU��������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u{ f1, f2, f3 };
                auto& a = *(*(asset.areaLights.end() - 1));
                a.u = u;
            }
            else if (token == "V") {  // ���ԴV��������
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                auto& a = *(*(asset.areaLights.end() - 1));
                a.v = v;
            }
            else if (token == "End") {  // ��Դ�������
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    // ���볡���ļ�
    // asset: �ʲ�������
    // path: �����ļ�·��
    // ����ֵ: �����Ƿ�ɹ�
    bool ScnImporter::import(Asset& asset, const string& path) {
        ifstream file(path);
        if (!file.is_open()) {
            lastErrorInfo = "File does not exist!";
            return false;
        }

        // ��¼����ǰ���ʲ�״̬
        size_t beginModel = asset.modelItems.size();
        size_t beginNode = asset.nodeItems.size();
        size_t beginMaterial = asset.materialItems.size();
        size_t beginTexture = asset.textureItems.size();

        size_t beginSph = asset.spheres.size();
        size_t beginTri = asset.triangles.size();
        size_t beginPln = asset.planes.size();
        size_t beginMsh = asset.meshes.size();

        size_t beginLight = asset.lightItems.size();
        size_t beginPnt = asset.pointLights.size();
        size_t beginArea = asset.areaLights.size();
        size_t beginDir = asset.directionalLights.size();
        size_t beginSpt = asset.spotLights.size();

        bool successFlag = true;

        string currline;
        stringstream ss{};

        map<string, size_t> mtlMap;  // �������Ƶ�������ӳ��
        
        // ���������ļ�
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token == "") continue;

            else if (token[0] == '#') continue;  // ����ע����

            else if (token == "Begin") {  // ��ʼ�����
                ss>>token;
                if (token == "Material") {  // ���ʶ����
                    successFlag = parseMtl(asset, file, mtlMap);
                }
                else if (token == "Model") {  // ģ�Ͷ����
                    successFlag = parseMdl(asset, file, mtlMap);
                }
                else if (token == "Light") {  // ��Դ�����
                    successFlag = parseLgt(asset, file);
                }
                else {
                    successFlag = false;
                }
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
            }

            ss.clear();
            ss.str("");
        }

        // Ϊ����ӵĽڵ�͹�Դ����OpenGLԤ��������
        if (successFlag) {
            for (auto i = beginNode; i < asset.nodeItems.size(); i++) {
                asset.genPreviewGlBuffersPerNode(asset.nodeItems[i]);
            }

            for (auto i = beginLight; i < asset.lightItems.size(); i++) {
                asset.genPreviewGlBuffersPerLight(asset.lightItems[i]);
            }
        }

        // �������ʧ�ܣ��ع����и���
        if (!successFlag) {
            asset.modelItems        .erase(asset.modelItems         .begin() + beginModel,      asset.modelItems.end());
            asset.nodeItems         .erase(asset.nodeItems          .begin() + beginNode,       asset.nodeItems.end());
            asset.materialItems     .erase(asset.materialItems      .begin() + beginMaterial,   asset.materialItems.end());
            asset.textureItems      .erase(asset.textureItems       .begin() + beginTexture,    asset.textureItems.end());
            
            asset.spheres           .erase(asset.spheres            .begin() + beginSph,        asset.spheres.end());
            asset.triangles         .erase(asset.triangles          .begin() + beginTri,        asset.triangles.end());
            asset.planes            .erase(asset.planes             .begin() + beginPln,        asset.planes.end());
            asset.meshes            .erase(asset.meshes             .begin() + beginMsh,        asset.meshes.end());
            
            asset.lightItems        .erase(asset.lightItems         .begin() + beginLight,      asset.lightItems.end());
            asset.pointLights       .erase(asset.pointLights        .begin() + beginPnt,        asset.pointLights.end());
            asset.areaLights        .erase(asset.areaLights         .begin() + beginArea,       asset.areaLights.end());
            asset.directionalLights .erase(asset.directionalLights  .begin() + beginDir,        asset.directionalLights.end());
            asset.spotLights        .erase(asset.spotLights         .begin() + beginSpt,        asset.spotLights.end());
        }

        return successFlag;
    }
}
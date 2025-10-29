#include "asset/Asset.hpp"

// �ʲ�����ʵ���ļ�
// ʵ���˳����ʲ���OpenGLԤ������

namespace NRenderer
{
    // Ϊÿ���ڵ�����OpenGLԤ��������
    // ���������������(VAO)�����㻺�����(VBO)�������������(EBO)
    void Asset::genPreviewGlBuffersPerNode(NodeItem& node) {
        // �����Ѵ��ڵĻ�����
        if (node.glVAO != 0) {
            glDeleteVertexArrays(1, &node.glVAO);
            node.glVAO = 0;
        }
        if (node.glVBO != 0) {
            glDeleteBuffers(1, &node.glVBO);
            node.glVBO = 0;
        }
        if (node.glEBO != 0) {
            glDeleteBuffers(1, &node.glEBO);
            node.glEBO = 0;
        }

        // �����µĶ����������
        glGenVertexArrays(1, &node.glVAO);
        glBindVertexArray(node.glVAO);
        auto& np = *node.node;
        using T = Node::Type;

        // ���ݽڵ��������ɲ�ͬ�Ļ�������
        if (np.type == T::TRIANGLE) {  // �����νڵ�
            auto& t = *triangles[np.entity];
            glGenBuffers(1, &node.glVBO);

            // ���ö�������
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*3, &t.v[0], GL_DYNAMIC_DRAW);
            
            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::SPHERE) {  // ����ڵ�
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            
            // ����������߿�����
            const int count = 60;
            // XYƽ��Բ
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            // XZƽ��Բ
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            // XZƽ�沿��Բ
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            // YZƽ��Բ
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }

            // ���ö�������
            glGenBuffers(1, &node.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {  // ƽ��ڵ�
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            
            // ����ƽ����ĸ�����
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);

            // ���ö�������
            glGenBuffers(1, &node.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::MESH) {  // ����ڵ�
            auto& m = *meshes[np.entity];
            
            // �������������������
            glGenBuffers(1, &node.glVBO);
            glGenBuffers(1, &node.glEBO);

            // ���ö�������
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*m.positions.size(), &m.positions[0], GL_DYNAMIC_DRAW);

            // ������������
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node.glEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* m.positionIndices.size(), &m.positionIndices[0], GL_DYNAMIC_DRAW);

            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        }

        glBindVertexArray(0);
    }

    // Ϊÿ����Դ����OpenGLԤ��������
    void Asset::genPreviewGlBuffersPerLight(LightItem& light) {
        // �����Ѵ��ڵĻ�����
        if (light.glVAO != 0) {
            glDeleteVertexArrays(1, &light.glVAO);
            light.glVAO = 0;
        }
        if (light.glVBO != 0) {
            glDeleteBuffers(1, &light.glVBO);
            light.glVBO = 0;
        }
        if (light.glEBO != 0) {
            glDeleteBuffers(1, &light.glEBO);
            light.glEBO = 0;
        }

        auto& lp = *light.light;
        using T = Light::Type;
        light.externalDrawData = make_shared<GlDrawData>();
        auto& ps = light.externalDrawData->positions;
        
        // ���������������
        glGenVertexArrays(1, &light.glVAO);
        glBindVertexArray(light.glVAO);

        // ���ݹ�Դ�������ɲ�ͬ�Ļ�������
        if (lp.type == T::POINT) {  // ���Դ
            const float l = 1.f/20.f;
            const int count = 8;
            // ���ɵ��Դ���߿��ʾ
            for (int i=0; i < count; i++) {
                Vec3 p{l*cos(float(i)/float(count)*2*3.1415926f), l*sin(float(i)/float(count)*2*3.1415926f), 0};
                ps.push_back({0, 0, 0});
                ps.push_back(p);
            }

            // ���ö�������
            glGenBuffers(1, &light.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_STATIC_DRAW);
            
            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::AREA) {  // ���Դ
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            // �������Դ���ĸ�����
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;

            // ���ö�������
            glGenBuffers(1, &light.glVBO);
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_DYNAMIC_DRAW);
            
            // ���ö�������
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::DIRECTIONAL) {  // ƽ�й�Դ
            // ��ʵ��
        }
        else if (lp.type == T::SPOT) {  // �۹��
            // ��ʵ��
        }
        glBindVertexArray(0);
    }

    // ���½ڵ��OpenGL��������
    void Asset::updateNodeGlDrawData(NodeItem& node) {
        auto& np = *node.node;
        using T = Node::Type;
        
        // ���ݽڵ����͸��²�ͬ������
        if (np.type == T::SPHERE) {  // ������������
            node.externalDrawData = make_shared<GlDrawData>();
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            
            // ��������������߿�����
            const int count = 60;
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }

            // ���»���������
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {  // ����ƽ������
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            
            // ��������ƽ��Ķ�������
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);

            // ���»���������
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::TRIANGLE) {  // ��������������
            auto& t = *triangles[np.entity];
            // ���»���������
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*3, &t.v[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // ���¹�Դ��OpenGL��������
    void Asset::updateLightGlDrawData(LightItem& light) {
        auto& lp = *light.light;
        using T = Light::Type;
        auto& ps = light.externalDrawData->positions;
        
        // ���ݹ�Դ���͸��²�ͬ������
        if (lp.type == T::POINT) {  // ���Դ
            // ���Դ����Ҫ����
        }
        else if (lp.type == T::AREA) {  // ���Դ
            light.externalDrawData = make_shared<GlDrawData>();
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            
            // �����������Դ�Ķ�������
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;

            // ���»���������
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*ps.size(), &ps[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::DIRECTIONAL) {  // ƽ�й�Դ
            // ��ʵ��
        }
        else if (lp.type == T::SPOT) {  // �۹��
            // ��ʵ��
        }
    }
}
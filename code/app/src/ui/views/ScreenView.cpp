#include "ui/views/ScreenView.hpp"
#include "server/Server.hpp"

// ��Ļ��ͼʵ���ļ�
// ʵ������Ⱦ�����Ԥ������ʾ����

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace NRenderer
{
    // ��ɫ��Դ���붨��
#pragma region __SHADER_SOURCE__
    // �ڵ㶥����ɫ��
    const char* nodeVShaderSource = 
        "#version 330 core                                                  \n"
        "layout (location = 0) in vec3 aPos;                                \n"
        "uniform mat4 model;                                                \n"
        "uniform mat4 view;                                                 \n"
        "uniform mat4 projection;                                           \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   gl_Position = projection*view*model*vec4(aPos.x, aPos.y, aPos.z, 1.0);      \n"
        "}                                                                  \n"
        ;
    // �ڵ�Ƭ����ɫ��
    const char* nodeFShaderSource = 
        "#version 330 core                                                  \n"
        "out vec4 FragColor;                                                \n"
        "uniform vec4 drawColor;                                            \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   FragColor = drawColor;                                          \n"
        "}                                                                  \n"
        ;
    // ��Դ������ɫ��
    const char* lightVShaderSource = 
        "#version 330 core                                                  \n"
        "layout (location = 0) in vec3 aPos;                                \n"
        "uniform mat4 model;                                                \n"
        "uniform mat4 view;                                                 \n"
        "uniform mat4 projection;                                           \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   mat4 vm = view;                                                 \n"
        "   gl_Position = projection*vm*model*vec4(aPos, 1.0);              \n"
        "}                                                                  \n"
        ;
    // ��ԴƬ����ɫ��
    const char* lightFShaderSource = 
        "#version 330 core                                                  \n"
        "out vec4 FragColor;                                                \n"
        "uniform vec4 drawColor;                                            \n"
        "void main()                                                        \n"
        "{                                                                  \n"
        "   FragColor = drawColor;                                          \n"
        "}                                                                  \n"
        ;
#pragma endregion __SHADER_SOURCE__

    // ���캯��
    // ��ʼ����Ļ��ͼ
    ScreenView::ScreenView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View                      (position, size, uiContext, manager)  // ���û��๹�캯��
        , renderResult              (0)                                   // ��Ⱦ�������ID
        , previewResult             (0)                                   // Ԥ���������ID
        , viewType                  (ViewType::PREVIEW)                   // Ĭ��ΪԤ��ģʽ
        , shrinkLevel               (0)                                   // Ĭ�����ż���
        , previewCoordinateType     (CoordinateType::LEFT_HANDED)        // Ĭ����������ϵ
        , nodeShader                (nodeVShaderSource, nodeFShaderSource)    // ��ʼ���ڵ���ɫ��
        , lightShader               (lightVShaderSource, lightFShaderSource)  // ��ʼ����Դ��ɫ��
    {
        // ��ʼ��OpenGL����
        glFBO = 0;    // ֡�������
        glRBO = 0;    // ��Ⱦ�������

        // ��ʼ��Ԥ���ߴ�
        prePreviewHeight = 0;
        prePreviewWidth = 0;
        previewHeight = 0;
        previewWidth = 0;
    }

    // ���Ԥ���ߴ��Ƿ�ı�
    bool ScreenView::isPreviewSizeChange() {
        bool f = (previewWidth != prePreviewWidth || previewHeight != prePreviewHeight);
        return f;
    }

    // ����Ԥ���ߴ�
    void ScreenView::updatePreviewSize(unsigned int x, unsigned int y) {
        previewHeight = y;
        previewWidth = x;
    }

    // ������һ�ε�Ԥ���ߴ�
    void ScreenView::updatePrePreviewSize() {
        prePreviewWidth = previewWidth;
        prePreviewHeight = previewHeight;
    }

    // ��ʼ���ƴ���
    void ScreenView::drawBeginWindow() {
        ImGui::Begin("Result", nullptr, windowFlag | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    // ��ȡ���ű���
    float ScreenView::getShrinkNum() {
        return 1.f - float(shrinkLevel)*0.125;
    }

    // ������Ļ����
    void ScreenView::draw() {
        // �������������С
        auto size = ImGui::GetWindowSize();
        size.y -= 65;
        size.x -= 10;

        // ������µ���Ⱦ������л��������ͼ
        if (getServer().screen.isUpdated()) viewType = ViewType::RESULT;

        // ��ʼ������������
        ImGui::BeginChild("Screen Content", size, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
        {
            if (viewType == ViewType::PREVIEW) {
                preview();  // ����Ԥ��
            }
            else if (viewType == ViewType::RESULT) {
                result();   // ���ƽ��
            }
        }
        ImGui::EndChild();
        ImGui::Separator();

        // ���ƿ��ư�ť
        if (viewType == ViewType::PREVIEW) {
            if (ImGui::Button("Preview", {80, 22})) viewType = ViewType::RESULT;
        }
        else if (viewType == ViewType::RESULT) {
            if (ImGui::Button("Result ", {80, 22})) viewType = ViewType::PREVIEW;
        }

        // ���ż���ѡ��
        ImGui::SameLine();
        const string shrinkLevelsLabel[8] = {
            "100.0%", "87.5%", "75.0%", "62.5%", "50.0%", "37.5%", "25.0%", "12.5%"
        };
        ImGui::SetNextItemWidth(80);
        if (ImGui::BeginCombo("##ShrinkLevel", shrinkLevelsLabel[shrinkLevel].c_str())) {
            for (int i=0; i < 8; i++) {
                bool selected = i == shrinkLevel;
                if (ImGui::Selectable(shrinkLevelsLabel[i].c_str(), &selected)) {
                    shrinkLevel = i;
                }
            }
            ImGui::EndCombo();
        }

        // ����ϵ�л���ť
        if (viewType == ViewType::PREVIEW) {
            ImGui::SameLine();
            if (previewCoordinateType == CoordinateType::LEFT_HANDED) {
                if (ImGui::Button("Left-handed", {100, 22})) previewCoordinateType = CoordinateType::RIGHT_HANDED;
            }
            else {
                if (ImGui::Button("Right-handed", {100, 22})) previewCoordinateType = CoordinateType::LEFT_HANDED;
            }
        }
    }

    // ����Ԥ����ͼ
    void ScreenView::preview() {
        // ����Ԥ���ߴ�
        auto ux = manager.renderSettingsManager.renderSettings.width;
        auto uy = manager.renderSettingsManager.renderSettings.height;
        float x = ux;
        float y = uy;
        x *= getShrinkNum();
        y *= getShrinkNum();
        updatePreviewSize(x, y);

        // ����ߴ�ı䣬��������֡����
        if (isPreviewSizeChange()) {
            genFB();
        }

        // ����Ԥ��ͼ��
        this->align({x, y});
        drawGlPreview();

        // ��������ϵ����������������
        ImVec2 bv = {1, 1}, ev = {0, 0};
        if (previewCoordinateType == CoordinateType::RIGHT_HANDED) {
            bv = {0, 1};
            ev = {1, 0};
        }
        ImGui::Image((void*)previewResult, { x, y }, bv, ev);
        updatePrePreviewSize();
    }

    // ������Ⱦ���
    void ScreenView::result() {
        Vec2 rs = {getServer().screen.getWidth(), getServer().screen.getHeight()};
        // ������µ���Ⱦ���
        if (getServer().screen.isUpdated()) {
            if (renderResult != 0) GlImage::deleteImage(renderResult);
            auto pixels = getServer().screen.getPixels();
            renderResult = GlImage::loadImage(pixels, rs);  // �����µ���Ⱦ���
        }
        rs *= getShrinkNum();  // Ӧ������
        this->align(rs);       // ����ͼ��
        ImGui::Image((void*)(intptr_t)renderResult, { rs.x, rs.y }, { 0, 0 }, { 1, 1 });
    }

    // ����ͼ�񵽴�������
    void ScreenView::align(const Vec2& size) {
        auto [x, y] = ImGui::GetWindowSize();
        if (size.x < x) {
            ImGui::SetCursorPosX((x-size.x)/2);  // ˮƽ����
        }
        if (size.y < y) {
            ImGui::SetCursorPosY((y-size.y)/2);  // ��ֱ����
        }
    }

    // ����OpenGL֡����
    void ScreenView::genFB() {
        auto& TX = previewResult;
        // ����ɵ�OpenGL����
        if (TX != 0) {
            glDeleteTextures(1, &TX);
            TX = 0;
        }
        if (glFBO != 0) {
            glDeleteFramebuffers(1, &glFBO);
            glFBO = 0;
        }
        if (glRBO != 0) {
            glDeleteRenderbuffers(1, &glRBO);
            glRBO = 0;
        }
        
        // ������ɫ����
        glGenTextures(1, &TX);
        glBindTexture(GL_TEXTURE_2D, TX);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // ���������Ⱦ����
        glGenRenderbuffers(1, &glRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, glRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, previewWidth, previewHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // ����������֡����
        glGenFramebuffers(1, &glFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewResult, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // ����OpenGLԤ��
    void ScreenView::drawGlPreview() {
        glBindFramebuffer(GL_FRAMEBUFFER, glFBO);
        nodeShader.use();

        // �����ӿں������ɫ
        glViewport(0, 0, previewWidth, previewHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // �����������
        auto& camera = manager.renderSettingsManager.camera;
        Mat4x4 projection = glm::perspective(glm::radians(camera.fov), camera.aspect, 0.1f, 10000000.0f);
        Mat4x4 view = glm::lookAt(camera.position, camera.lookAt, camera.up);

        // ������ɫ������
        nodeShader.setMat4x4("projection", projection);
        nodeShader.setMat4x4("view", view);

        // ������ɫ
        const Vec4 commonColor{1, 1, 0, 1};      // ��ͨ�ڵ���ɫ
        const Vec4 selectedColor{1, 0.5, 0.5, 1}; // ѡ�нڵ���ɫ

        // ���Ƴ����ڵ�
        auto& asset = manager.assetManager.asset;
        for (int i=0; i<asset.nodeItems.size(); i++) {
            auto& ni = asset.nodeItems[i];
            if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_MODEL) {
                if (uiContext.previewModel != ni.node->model) {
                    nodeShader.setVec4("drawColor", commonColor);
                    previewNode(ni);
                }
            }
            else if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_NODE) {
                if (uiContext.previewNode != i) {    
                    nodeShader.setVec4("drawColor", commonColor);
                    previewNode(ni);
                }
            }
            else {
                nodeShader.setVec4("drawColor", commonColor);
                previewNode(ni);
            }
        }
        if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_MODEL) {
            if (uiContext.previewModel != -1 && uiContext.previewModel < asset.modelItems.size()) {
                auto& m = *asset.modelItems[uiContext.previewModel].model;
                for (auto& nIdx : m.nodes) {
                    nodeShader.setVec4("drawColor", selectedColor);
                    previewNode(asset.nodeItems[nIdx]);
                }
            }
        }
        else if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_NODE) {
            if (uiContext.previewNode != -1 && uiContext.previewNode < asset.nodeItems.size()) {
                nodeShader.setVec4("drawColor", selectedColor);
                previewNode(asset.nodeItems[uiContext.previewNode]);
            }
        }

        lightShader.use();
        lightShader.setMat4x4("projection", projection);
        lightShader.setMat4x4("view", view);

        const Vec4 commonLightColor{1, 1, 1, 1};
        const Vec4 selectedLightColor{0.5, 0.5, 1, 1};

        for (int i=0; i < asset.lightItems.size(); i++) {
            auto& l = asset.lightItems[i];
            if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_LIGHT) {
                if (i != uiContext.previewLight) {
                    lightShader.setVec4("drawColor", commonLightColor);
                    previewLight(l);
                }
            }
            else {
                lightShader.setVec4("drawColor", commonLightColor);
                previewLight(l);
            }
        }
        if (uiContext.previewMode == UIContext::PreviewMode::PREVIEW_LIGHT) {
            if (uiContext.previewLight != -1 && uiContext.previewLight < asset.lightItems.size()) {
                lightShader.setVec4("drawColor", selectedLightColor);
                previewLight(asset.lightItems[uiContext.previewLight]);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ScreenView::previewNode(const NodeItem& n) {
        auto& camera = manager.renderSettingsManager.camera;
        auto& asset = manager.assetManager.asset;
        glBindVertexArray(n.glVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Mat4x4 modelMat{1};
        auto& model = *asset.modelItems[n.node->model].model;
        if (n.node->type == Node::Type::TRIANGLE) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        else if (n.node->type == Node::Type::PLANE) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else if (n.node->type == Node::Type::SPHERE) {
            const Vec3 norm{0, 0, -1};
            Vec3 pos = asset.spheres[n.node->entity]->position + model.translation;
            Vec3 dir = camera.position - camera.lookAt;
            dir = -glm::normalize(dir);
            float cos_theta = glm::dot(dir, norm);
            modelMat = glm::translate(modelMat, pos);
            modelMat = glm::scale(modelMat, model.scale);
            if (dir != norm && dir != -norm) {
                Vec3 axis = glm::cross(dir, norm);
                float angle = -acos(cos_theta);
                modelMat = glm::rotate(modelMat,angle, axis);
            }
            nodeShader.setMat4x4("model", modelMat);
            glDrawArrays(GL_LINE_STRIP, 0, n.externalDrawData->positions.size());
        }
        else if (n.node->type == Node::Type::MESH) {
            modelMat = glm::translate(modelMat, model.translation);
            modelMat = glm::scale(modelMat, model.scale);
            nodeShader.setMat4x4("model", modelMat);
            auto& m = *asset.meshes[n.node->entity];
            glDrawElements(GL_TRIANGLES, m.positionIndices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    void ScreenView::previewLight(const LightItem& l) {
        auto& camera = manager.renderSettingsManager.camera;
        auto& asset = manager.assetManager.asset;
        glBindVertexArray(l.glVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Mat4x4 model{1};
        if (l.light->type == Light::Type::AREA) {
            model = glm::translate(model, asset.areaLights[l.light->entity]->position);
            lightShader.setMat4x4("model", model);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else if (l.light->type == Light::Type::POINT) {
            const Vec3 n{0, 0, -1};
            Vec3 dir = camera.position - camera.lookAt;
            float dis = glm::length(camera.position - asset.pointLights[l.light->entity]->position);
            float delta = 1/tan(glm::radians(camera.fov/2));
            float scale = (dis + delta) / delta;
            dir = -glm::normalize(dir);
            float cos_theta = glm::dot(dir, n);
            model = glm::translate(model, asset.pointLights[l.light->entity]->position);
            model = glm::scale(model, Vec3{scale});
            if (dir != n && dir != -n) {
                Vec3 axis = glm::cross(dir, n);
                float angle = -acos(cos_theta);
                model = glm::rotate(model, angle, axis);
            }
            lightShader.setMat4x4("model", model);
            glDrawArrays(GL_LINES, 0, 16);
        }
        glBindVertexArray(0);
    }

} // namespace NRenderer

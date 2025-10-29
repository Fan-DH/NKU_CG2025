#pragma once
#ifndef __NR_GL_IMAGE_HPP__
#define __NR_GL_IMAGE_HPP__

// OpenGLͼ�񹤾���ͷ�ļ�
// �ṩ��OpenGL����ļ��غ͹�����

#include "geometry/vec.hpp"
#include "glad/glad.h"

namespace NRenderer
{
    // OpenGL����ID����
    using GlImageId = unsigned int;

    // OpenGLͼ�񹤾���
    // �ṩ�˼��غ�ɾ��OpenGL����ľ�̬����
    class GlImage
    {
    private:
    public:
        // ����RGBA��ʽͼ��OpenGL����
        // pixels: RGBA��ʽ����������
        // size: ͼ��ߴ磨��Ⱥ͸߶ȣ�
        // �������ɵ�����ID
        static GlImageId loadImage(const RGBA* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            // ����������˲���
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // �ϴ���������
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }

        // ɾ��OpenGL����
        // id: Ҫɾ��������ID
        static void deleteImage(GlImageId id) {
            glDeleteTextures(1, &id);
        }

        // ����RGB��ʽͼ��OpenGL����
        // pixels: RGB��ʽ����������
        // size: ͼ��ߴ磨��Ⱥ͸߶ȣ�
        // �������ɵ�����ID
        static GlImageId loadImage(const RGB* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            // ����������˲���
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // �ϴ���������
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGB, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }
    };    
} // namespace NRenderer

#endif
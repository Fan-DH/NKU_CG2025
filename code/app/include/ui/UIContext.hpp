#pragma once
#ifndef __NR_UI_CONTEXT_HPP__
#define __NR_UI_CONTEXT_HPP__

// UI������ͷ�ļ�
// ������UI��״̬��Ԥ��ģʽ

#include "geometry/vec.hpp"

namespace NRenderer
{
    // UI�����Ľṹ��
    // ����UI��״̬��Ԥ����Ϣ
    struct UIContext
    {
        // UI״̬ö��
        enum class State
        {
            HOVER_COMPONENT_PROGRESS,  // ��ͣ�������������
            NORMAL                     // ����״̬
        };
        State state;                   // ��ǰUI״̬

        // Ԥ��ģʽö��
        enum class PreviewMode {
            PREVIEW_NONE,              // ��Ԥ��
            PREVIEW_MODEL,             // Ԥ��ģ��
            PREVIEW_NODE,              // Ԥ���ڵ�
            PREVIEW_LIGHT              // Ԥ����Դ
        };

        PreviewMode previewMode;       // ��ǰԤ��ģʽ

        Index previewModel;            // Ԥ����ģ������
        Index previewNode;             // Ԥ���Ľڵ�����
        Index previewLight;            // Ԥ���Ĺ�Դ����

        // ���캯��
        // ��ʼ��UI������ΪĬ��״̬
        UIContext()
            : state (State::NORMAL)                    // ��ʼΪ����״̬
            , previewNode(-1)                          // ��Ԥ���ڵ�
            , previewLight(-1)                         // ��Ԥ����Դ
            , previewModel(-1)                         // ��Ԥ��ģ��
            , previewMode(PreviewMode::PREVIEW_NONE)   // ��Ԥ��ģʽ
        {}

        // ��ȡ��ǰ״̬
        // ����: ��ǰUI״̬
        inline
        State getState() const {
            return state;
        }
    };
} // namespace NRenderer


#endif
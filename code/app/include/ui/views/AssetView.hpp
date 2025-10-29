#pragma once
#ifndef __NR_ASSET_VIEW_HPP__
#define __NR_ASSET_VIEW_HPP__

// �ʲ���ͼͷ�ļ�
// �����˳����ʲ��Ĺ���ͱ༭����

#include "View.hpp"

namespace NRenderer
{
    // �ʲ���ͼ��
    // ������ʾ�ͱ༭�����е�ģ�͡����ʡ�����͹�Դ
    class AssetView: public View
    {
    private:
        // ��д�����麯��
        virtual void drawBeginWindow() override;  // ��ʼ���ƴ���
        virtual void draw() override;            // ��������

        void menu();                            // ���Ʋ˵�
        void modelTab();                        // ģ�ͱ�ǩҳ
        void materialTab();                     // ���ʱ�ǩҳ
        void textureTab();                      // �����ǩҳ
        void lightTab();                        // ��Դ��ǩҳ

        int currMtlIndex;                       // ��ǰѡ�еĲ�������
        int currLightIndex;                     // ��ǰѡ�еĹ�Դ����

        Property tempProp;                      // ��ʱ����
        int tempPropTypeIndex;                  // ��ʱ������������
        bool isPropKeyValid;                    // ���Լ��Ƿ���Ч
        void tempPropEditor();                  // ��ʱ���Ա༭��

        void resetTempProp();                   // ������ʱ����

        MaterialItem tempMaterialItem;          // ��ʱ������

        // �������Ա༭��
        // spMaterial: Ҫ�༭�Ĳ���ָ��
        void materialPropEditor(SharedMaterial spMaterial);

    public:
        // ���캯��
        // position: ��ͼλ��
        // size: ��ͼ��С
        // uiContext: UI������
        // manager: ������
        AssetView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager);
    };
} // namespace NRenderer


#endif
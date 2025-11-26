#pragma once
#ifndef __VERTEX_TRANSFORM_HPP__
#define __VERTEX_TRANSFORM_HPP__

#include "scene/Scene.hpp"

namespace PhotonMapping
{
    using namespace NRenderer;
    
    /**
     * ����任��
     * ���𽫳����еļ�����Ӿֲ�����ϵת������������ϵ
     * Ӧ��ģ�͵ı任����ƽ�ơ���ת�����ţ�
     */
    class VertexTransformer
    {
    private:
    public:
        /**
         * ִ�ж���任
         * �����������м�����Ķ���Ӿֲ�����ת������������
         * @param spScene ��������ָ��
         */
        void exec(SharedScene spScene);
    };
}

#endif
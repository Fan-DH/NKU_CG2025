#pragma once
#ifndef __NR_IMAGE_LOADER_HPP__
#define __NR_IMAGE_LOADER_HPP__

// ͼ�������ͷ�ļ�
// �ṩ�˴��ļ�����ͼ��Ĺ���

#include <string>

#include "geometry/vec.hpp"
#include "Image.hpp"

namespace NRenderer {
	using namespace std;

    // ͼ���������
    // ������ļ�ϵͳ����ͼ������
	class ImageLoader
	{
	public:
        // Ĭ�Ϲ��캯��
		ImageLoader() = default;
        // Ĭ����������
		~ImageLoader() = default;

        // ����ͼ���ļ�
        // file: ͼ���ļ�·��
        // channel: ������ͼ��ͨ������Ĭ��Ϊ4����RGBA��
        // ���ؼ��ص�ͼ�����ݣ��������ʧ�ܷ���nullptr
		Image* load(const string& file, int channel = 4);
	};
}

#endif
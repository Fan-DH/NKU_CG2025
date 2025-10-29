#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utilities/ImageLoader.hpp"

// ͼ�������ʵ���ļ�
// ʹ��stb_image�����ͼ���ļ�

namespace NRenderer
{
	// ����ͼ���ļ�
	// file: ͼ���ļ�·��
	// channel: ������ͨ������3=RGB��4=RGBA��
	// ����ֵ: ���ص�ͼ�����ݣ�����ʧ�ܷ���nullptr
	Image* ImageLoader::load(const string& file, int channel) {
		// ���ͨ�����Ƿ���Ч
		if (channel != 3 && channel != 4) return nullptr;

		// ����ͼ�����
		Image* image = new Image();

		// ʹ��stb_image����ͼ��
		auto data = stbi_load(file.c_str(), &(image->width), &(image->height), &(image->channel), channel);
		image->channel = channel;

		// ��ͼ������ת��Ϊ��������ʽ��0-1��Χ��
		image->data = new float[image->width * image->height * image->channel];
		for (unsigned long i = 0; i < image->height * image->width * channel; i++) {
			image->data[i] = (float)(data[i]) / 255.f;  // ��8λ����ת��Ϊ������
		}

		// �ͷ�stb_image������ڴ�
		stbi_image_free(data);

		return image;
	}
}
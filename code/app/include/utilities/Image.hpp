#pragma once
#ifndef __NR_IMAGE_HPP__
#define __NR_IMAGE_HPP__

// ͼ�����ݽṹͷ�ļ�
// �����˴洢�͹���ͼ�����ݵĻ����ṹ

namespace NRenderer
{
    using namespace std;

    // ͼ�����ݽṹ
    // �洢ͼ��ĳߴ硢ͨ��������������
	struct Image {
		int height;     // ͼ��߶�
		int width;      // ͼ����
		int channel;    // ͼ��ͨ����
		float* data;    // �������ݣ���Χ��0.0 - 1.0��

        // Ĭ�Ϲ��캯��
        // ����һ����ͼ��
		Image()
			: height(0)
			, width(0)
			, channel(0)
			, data(nullptr)
		{}

        // �������캯��
        // image: Դͼ��
        // ����һ��Դͼ������
        Image(const Image& image) {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = new float[size];
            for (int i = 0; i < size; i++) {
                this->data[i] = image.data[i];
            }
        }

        // �ƶ����캯��
        // image: Դͼ��
        // ��Դͼ���ȡ��Դ����Ȩ
        Image(Image&& image) noexcept {
            this->height = image.height;
            this->width = image.width;
            this->channel = image.channel;
            int size = image.height*image.width*image.channel;
            this->data = image.data;
            image.data = nullptr;  // Դͼ���������Ȩ
        }

        // ��������
        // �ͷ����������ڴ�
        ~Image() {
            delete[] data;
        }
	};
}

#endif
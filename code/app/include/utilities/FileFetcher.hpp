#pragma once
#ifndef __NR_FILE_FETCHER_HPP__
#define __NR_FILE_FETCHER_HPP__

// �ļ���ȡ��ͷ�ļ�
// �ṩ���ļ�ѡ��ͻ�ȡ�Ĺ���

#include <optional>
#include <string>

namespace NRenderer
{
    using namespace std;

    // �ļ���ȡ����
    // ���ڴ��ļ�ѡ��Ի��򲢻�ȡ�û�ѡ����ļ�·��
    class FileFetcher
    {
    public:
        // Ĭ�Ϲ��캯��
        FileFetcher() = default;
        // Ĭ����������
        ~FileFetcher() = default;

        // ��ȡ�ļ�·��
        // filter: �ļ��������ַ������� "*.obj"��
        // �����û�ѡ����ļ�·��������û�ȡ���򷵻ؿ�
        optional<string> fetch(const char* filter) const;
    };
} // namespace NRenderer

#endif
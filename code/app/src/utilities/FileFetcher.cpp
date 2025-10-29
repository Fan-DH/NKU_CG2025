#include "utilities/FileFetcher.hpp"
#include "Windows.h"
#include "commdlg.h"

// �ļ���ȡ��ʵ���ļ�
// �ṩWindows�ļ�ѡ��Ի�����

namespace NRenderer
{
    using namespace std;

    // ���ļ�ѡ��Ի���
    // filter: �ļ��������ַ������� "All(*.obj *.scn)\0 *.obj;*.scn\0"
    // ����ֵ: ѡ���ļ���·�������ȡ���򷵻�nullopt
    optional<string> FileFetcher::fetch(const char* filter) const
    {
        OPENFILENAME ofn;              // ͨ�öԻ���ṹ��
        TCHAR szFile[260];             // �ļ���������
        HWND hwnd = GetActiveWindow(); // �����ߴ��ھ��
        HANDLE hf;                     // �ļ����

        // ��ʼ��OPENFILENAME�ṹ��
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        
        // ��lpstrFile[0]����Ϊ'\0'������GetOpenFileName����ʹ��szFile�����ݽ��г�ʼ��
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        
        // �����ļ�������
        // ʾ��: "All(*.obj *.scn)\0 *.obj;*.scn\0.obj file\0*.obj\0custom scene file\0*.scn\0"
        ofn.lpstrFilter = (filter);
        ofn.nFilterIndex = 1;          // Ĭ��ѡ���һ��������
        
        // ��������
        ofn.lpstrFileTitle = NULL;     // ����Ҫ�ļ�����
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;    // ʹ��Ĭ��Ŀ¼
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  // �ļ���·���������

        // ��ʾ�Ի��򲢴�����
        if (GetOpenFileName(&ofn) == TRUE)
        {
            string filePath{(char *)ofn.lpstrFile};  // ��ȡѡ�е��ļ�·��
            return filePath;                         // �����ļ�·��
        }
        else {
            return nullopt;                          // �û�ȡ��������nullopt
        }
    }
}
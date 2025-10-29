#include "manager/Manager.hpp"
#include "server/Server.hpp"

// ������ʵ���ļ�
// ���������Ⱦ���á��ʲ������

namespace NRenderer
{
    // ���캯��
    // ��ʼ�������ӹ��������������
    Manager::Manager()
        : renderSettingsManager             ()  // ��Ⱦ���ù�����
        , assetManager                      ()  // �ʲ�������
        , componentManager                  ()  // ���������
    {
        componentManager.init(".\\components\\*.dll");  // ��ʼ�����������������DLL���
   }
}
// ����ģ��ʵ���ļ�
// �ṩ����ģ��ľ�̬��Ա��������ͳ�ʼ��

#include "templates/MaterialTemplates.hpp"

using namespace NRenderer;
using namespace std;

// ����ģ��ӳ�����Ϊ�������ͣ�ֵΪ��Ӧ��ģ��
unordered_map<int, MaterialTemplates::Template> MaterialTemplates::templates{};

// ����ģ���ʼ�����������ڳ�������ʱ��ʼ������Ԥ����Ĳ���ģ��
MaterialTemplates::InitTemplates MaterialTemplates::initTemplates = MaterialTemplates::InitTemplates{MaterialTemplates::templates};
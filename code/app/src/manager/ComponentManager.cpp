#include "manager/ComponentManager.hpp"
#include "io.h"

// ���������ʵ���ļ�
// ���������Ⱦ����ļ��ء�ִ�к�״̬

namespace NRenderer
{
    // ���캯��
    // ��ʼ�������������״̬�ͳ�Ա����
    ComponentManager::ComponentManager()
        : state             (State::IDLING)         // ��ʼ״̬Ϊ����
        , loadedDlls        ()                      // �Ѽ��ص�DLL�б�
        , activeComponent   ()                      // ��ǰ����
        , lastStartTime     ()                      // �ϴ�ִ�п�ʼʱ��
        , lastEndTime       ()                      // �ϴ�ִ�н���ʱ��
        , t                 ()                      // ִ���߳�
    {}

    // ��ʼ�����������
    // path: DLL�ļ�������·��
    void ComponentManager::init(const string& path) {
        auto& f = getServer().componentFactory;
        _finddata_t findData;
        
        // ��������������DLL�ļ�
        auto handle = _findfirst(path.c_str(), &findData);
        if (handle == -1) return;
        do {
            string name{findData.name};
            string filePath = (path.substr(0 ,path.size() - 6) + "\\" + name);
            
            // ����DLL�ļ�
            auto h = ::LoadLibrary((LPCSTR)filePath.c_str());
            if (h != NULL) {
                loadedDlls.push_back(h);  // ����DLL���
            }
        } while (_findnext(handle, &findData) == 0);
        _findclose(handle);
    }

    // ������ִ��
    // ��״̬����Ϊ����
    void ComponentManager::finish() {
        state = State::IDLING;
    }

    // ��ȡ��ǰ��������Ϣ
    // ����ֵ: ��������Ϣ
    ComponentInfo ComponentManager::getActiveComponentInfo() const {
        return activeComponent;
    }

    // ��ȡ��ǰ�����������״̬
    // ����ֵ: ��ǰ״̬
    ComponentManager::State ComponentManager::getState() const {
        return state;
    }

    // ��ȡ�ϴ�ִ�е�ʱ��
    // ����ֵ: ִ�г���ʱ��
    chrono::duration<double> ComponentManager::getLastExecTime() const {
        return lastEndTime - lastStartTime;
    }

    // ��������
    // �ͷ����м��ص�DLL
    ComponentManager::~ComponentManager()
    {
        for (auto& h : loadedDlls) {
            ::FreeLibrary(h);  // �ͷ�DLL
        }
    }

} // namespace NRenderer

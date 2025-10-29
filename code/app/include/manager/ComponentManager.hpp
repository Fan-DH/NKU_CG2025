#pragma once
#ifndef __NR_COMPONENT_MANAGER_HPP__
#define __NR_COMPONENT_MANAGER_HPP__

// ���������ͷ�ļ�
// �����˹�����Ⱦ����Ĺ�����

#include <Windows.h>
#include <vector>
#include <chrono>
#include <thread>

#include "component/RenderComponent.hpp"
#include "server/Server.hpp"

namespace NRenderer
{
    using namespace std;

    // �����������
    // ��������ִ����Ⱦ���
    class DLL_EXPORT ComponentManager
    {
    public:
        // ���״̬ö��
        enum class State
        {
            IDLING,     // ����״̬
            READY,      // ����״̬
            RUNNING,    // ����״̬
            FINISH      // ���״̬
        };
    private:
        State state;                    // ��ǰ״̬
        vector<HMODULE> loadedDlls;     // �Ѽ��ص�DLLģ���б�
        ComponentInfo activeComponent;   // ��ǰ������Ϣ
        chrono::system_clock::time_point lastStartTime;  // �ϴο�ʼʱ��
        chrono::system_clock::time_point lastEndTime;    // �ϴν���ʱ��
        thread t;                       // ִ���߳�

    public:
        // ���캯��
        ComponentManager();
        // ��������
        ~ComponentManager();

        // ��ʼ�����������
        // dllPath: DLL�ļ�·��
        void init(const string& dllPath);
        
        // ��ȡ��ǰ������Ϣ
        // ����: ��������Ϣ
        ComponentInfo getActiveComponentInfo() const;
        
        // ִ�����
        // Interface: ����ӿ�����
        // componentInfo: �����Ϣ
        // args: ���ִ�в���
        template<typename Interface, typename ...Args>
        void exec(const ComponentInfo& componentInfo, Args... args) {
            auto component = getServer().componentFactory.createComponent<Interface>(componentInfo.type, componentInfo.name);
            activeComponent = componentInfo;
            this->state = State::READY;
            try {
                t = thread(&Interface::exec,
                    component, 
                    [this]() -> void {
                        this->state = State::RUNNING;
                        this->lastStartTime = chrono::system_clock::now();
                    },
                    [this]() -> void {
                        this->state = State::FINISH;
                        this->lastEndTime = chrono::system_clock::now();
                    },
                    std::forward<Args>(args)...);
                    t.detach();
            }
            catch(const exception& e) {
                cerr<<"Unexpected termination"<<endl;
                cerr<<e.what()<<endl;
            }
        }
    
        // ������ִ��
        void finish();

        // ��ȡ��ǰ״̬
        // ����: ��ǰ���״̬
        State getState() const;

        // ��ȡ�ϴ�ִ��ʱ��
        // ����: �ϴ�ִ�еĳ���ʱ��
        chrono::duration<double> getLastExecTime() const;
    };
} // namespace NRenderer


#endif
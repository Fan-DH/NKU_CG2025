// ��־ϵͳ�ඨ��
// �ṩ�̰߳�ȫ����־��¼���ܣ�֧�ֲ�ͬ���͵���־��Ϣ
#pragma once
#ifndef __NR_LOGGER_HPP__
#define __NR_LOGGER_HPP__

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>
#include <mutex>

#include "common/macros.hpp"

#undef ERROR

namespace NRenderer
{
    using namespace std;
    // ��־��¼����
    // ����ϵͳ��־��֧�ֶ�����־���ͺ��̰߳�ȫ����־��¼
    class DLL_EXPORT Logger
    {
    public:
        // ��־����ö��
        // �����˲�ͬ�������־��Ϣ
        enum class LogType
        {
            NORMAL,     // ��ͨ��Ϣ
            WARNING,    // ������Ϣ
            ERROR,      // ������Ϣ
            SUCCESS     // �ɹ���Ϣ
        };

        // ��־�ı��ṹ��
        // �洢������־�����ͺ�����
        struct LogText
        {
            LogType type;    // ��־����
            string message;  // ��־����

            LogText() = delete;
            // ������ͨ���͵���־
            LogText(const string& str)
                : type          (LogType::NORMAL)
                , message       (str)
            {}
            // ����ָ�����͵���־
            LogText(LogType type, const string& str)
                : type          (type)
                , message       (str)
            {}
        };
    private:
        vector<LogText> msgs;    // ��־��Ϣ�б�
        mutex   mtx;             // ����������֤�̰߳�ȫ

    public:
        // Ĭ�Ϲ��캯��
        Logger();
        ~Logger() = default;
        // ���ÿ������ƶ�����
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;

        // ��¼ָ�����͵���־
        void log(const string& msg, LogType type);

        // ��¼��ͨ��־
        void log(const string& msg);

        // ��¼������־
        void warning(const string& msg);

        // ��¼������־
        void error(const string& msg);

        // ��¼�ɹ���־
        void success(const string& msg);

        // ���������־
        void clear();

        // ��־��Ϣ���Ͻṹ��
        // ����������ȡ��־��Ϣ
        struct LogMessages
        {
            unsigned nums;   // ��־��Ϣ����
            LogText* msgs;   // ��־��Ϣ����
        };

        // ��ȡ������־��Ϣ
        LogMessages get();
    };
    using SharedLogger = shared_ptr<Logger>;  // ��־��¼��������ָ������
} // namespace NRenderer


#endif
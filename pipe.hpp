#pragma once
// QT
#include <QThread>

// WINAPI
#include <windows.h>

// C++ STL
#include <functional>

#define PIPE_BUFFER_SZ 1024

class pipe : public QThread {
    Q_OBJECT
    //TEXT("\\\\.\\pipe\\Pipe"
public:
    pipe(LPCWSTR pipe_name) {
        hPipe = CreateNamedPipe(pipe_name,
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                1,
                                PIPE_BUFFER_SZ * sizeof(char),
                                PIPE_BUFFER_SZ * sizeof(char),
                                NMPWAIT_USE_DEFAULT_WAIT,
                                NULL);
    }
    void set_handler_fun(std::function<void(char*)> _fun_h) {
        m_fun_h = std::move(_fun_h);
    }
    void set_delay_ms(UINT _del) {
        m_delay = _del;
    }

    void run() override {
        char buffer[PIPE_BUFFER_SZ];
        DWORD dwRead;

        while (hPipe != INVALID_HANDLE_VALUE) {
            if (ConnectNamedPipe(hPipe, NULL) != FALSE) {
                while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) !=
                       FALSE) {
                    buffer[dwRead] = '\n';
                    m_fun_h(buffer);
                }
                Sleep(m_delay);
            }

            DisconnectNamedPipe(hPipe);
        }
    }

private:
    HANDLE hPipe;
    std::function<void(char* m_fun_arg_cstr)> m_fun_h;
    DWORD m_delay = 0;
};

//
// Created by Administrator on 2024/3/23.
//

#include "define.h"
#include "windows_api.h"

namespace tinyShell
{
#ifdef WINDOWS_OS
    std::string basename(const std::string &path)
    {
        // 找到最后一个路径分隔符的位置
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos)
        {
            // 返回路径中最后一个分隔符后面的部分作为文件名
            return path.substr(pos + 1);
        } else
        {
            // 如果路径中没有分隔符，直接返回原始路径
            return path;
        }
    }

    int tcgetattr(int fd, Termios *termios_p)
    {
        if (!termios_p) return -1;
        termios_p->c_cflag = 0;
        // 在Windows中无法直接获取终端属性，这里简单设置为0
        return 0;
    }

    int tcsetattr(int fd, int optional_actions, const Termios *termios_p)
    {
        if (!termios_p) return -1;
        // 设置控制台模式
        return SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), termios_p->c_cflag) ? 0 : -1;
    }

    int pipe(int pfd[2])
    {
        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES saAttr;

// 设置安全属性以允许句柄被子进程继承
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

// 创建管道
        if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0))
        {
            std::cerr << "CreatePipe failed" << std::endl;
            return -1;
        }

// 将句柄保存到数组中
        pfd[0] = _open_osfhandle((intptr_t) hReadPipe, 0);
        pfd[1] = _open_osfhandle((intptr_t) hWritePipe, 0);

        return 0;
    }

    int fork()
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // 创建子进程
        if (!CreateProcess(
                nullptr,                   // 模块名
                (char *) "child_process.exe",    // 命令行
                nullptr,                   // 进程句柄不可继承
                nullptr,                   // 线程句柄不可继承
                FALSE,                  // 不继承句柄
                0,                      // 无特殊标志
                nullptr,                   // 使用父进程的环境
                nullptr,                   // 使用父进程的当前目录
                &si,                    // 启动信息
                &pi                     // 进程信息
        ))
        {
            std::cerr << "CreateProcess failed" << std::endl;
            return -1;
        }

        // 父进程返回子进程的PID
        return (int) pi.dwProcessId;
    }

    void execl(const std::string &nop1,
               const std::string &nop2,
               const std::string &nop3,
               const std::string &commandLine,
               char *flag)
    {
        // 创建进程
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(
                "C:\\Windows\\System32\\bash.exe",  // 可执行文件路径
                (LPSTR) commandLine.c_str(),         // 命令行参数
                NULL,                               // 进程句柄不可继承
                NULL,                               // 线程句柄不可继承
                FALSE,                              // 不继承句柄
                0,                                  // 无特殊标志
                NULL,                               // 使用父进程的环境
                NULL,                               // 使用父进程的当前目录
                &si,                                // 启动信息
                &pi                                 // 进程信息
        ))
        {
            std::cerr << "CreateProcess failed" << std::endl;
            return;
        }

        // 等待子进程结束
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 关闭进程和线程的句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    pid_t getpid()
    {
        return GetCurrentProcessId(); // 获取当前进程的标识符
    }

    void setenv(const CHAR *key, const CHAR *value, int ops)
    {
        SetEnvironmentVariable((LPCSTR) key, (LPCSTR) value);
    }

    int chdir(const std::string &path)
    {
        return SetCurrentDirectory(path.c_str()) ? 0 : -1;
    }

    int gettimeofday(struct timeval *tv, void *tz)
    {
        FILETIME ft;
        ULARGE_INTEGER ularge;
        ULONGLONG usec;

        // 获取当前系统时间
        GetSystemTimeAsFileTime(&ft);

        // 将 FILETIME 转换为微秒
        ularge.LowPart = ft.dwLowDateTime;
        ularge.HighPart = ft.dwHighDateTime;
        usec = ularge.QuadPart / 10 - 11644473600000000ULL;

        // 填充 timeval 结构体
        tv->tv_sec = (long) (usec / 1000000);
        tv->tv_usec = (long) (usec % 1000000);
        return 0;
    }

    int waitpid(DWORD pid, int *status, int options)
    {
        HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (hProcess == NULL)
        {
            std::cerr << "OpenProcess failed" << std::endl;
            return -1;
        }

        DWORD dwExitCode;
        DWORD result = WaitForSingleObject(hProcess, INFINITE);
        if (result == WAIT_OBJECT_0 && GetExitCodeProcess(hProcess, &dwExitCode))
        {
            if (status != nullptr)
            {
                *status = (int) dwExitCode;
            }
        } else
        {
            std::cerr << "WaitForSingleObject failed" << std::endl;
        }

        CloseHandle(hProcess);
        return result == WAIT_OBJECT_0 ? 0 : -1;
    }
#endif
}

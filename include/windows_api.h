//
// Created by Administrator on 2024/3/23.
//

#ifndef TINY_SHELL_WINDOWS_API_H
#define TINY_SHELL_WINDOWS_API_H

#ifdef WINDOWS_OS
#include <Windows.h>
#include <string>
#include <iostream>
#include <shlwapi.h>
#include <io.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define TCSANOW 0
#define ICANON ENABLE_ECHO_INPUT
#define ECHO ENABLE_LINE_INPUT

#define WIFEXITED(status) (((status) & 0x7f) == 0)
#define WEXITSTATUS(status) (((status) >> 8) & 0xff)

#pragma comment(lib, "Shell32.lib") // 引入 Shell32.lib 库

namespace tinyShell
{
    struct Termios
    {
        // DWORD mode;
        int c_cflag;
    };

    typedef DWORD pid_t; // 定义 pid_t 类型为 DWORD

    extern std::string basename(const std::string &path);

    extern int tcgetattr(int fd, Termios *termios_p);

    extern int tcsetattr(int fd, int optional_actions, const Termios *termios_p);

    extern pid_t getpid();

    extern void setenv(const CHAR *key, const CHAR *value, int ops);

    extern int chdir(const std::string &path);

    extern int gettimeofday(struct timeval *tv, void *tz);

    extern int waitpid(DWORD pid, int *status, int options);

    extern int pipe(int pfd[2]);

    extern int fork();

    extern void execl(const std::string &nop1,
                      const std::string &nop2,
                      const std::string &nop3,
                      const std::string &commandLine,
                      char *flag);
}
#endif

#endif //TINY_SHELL_WINDOWS_API_H

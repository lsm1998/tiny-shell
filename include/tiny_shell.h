//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_TINY_SHELL_H
#define TINY_SHELL_TINY_SHELL_H

#include "command.h"
#include "state.h"
#include "system_api.h"

class TinyShell
{
private:
    std::unordered_map<String, String> envs = {};  // 环境变量列表
    struct termios oldAttr{}, newAttr{};  // 终端属性

public:
    explicit TinyShell() : TinyShell(0, nullptr)
    {};

    explicit TinyShell(int argc, char **argv);

    int loopHandler();

private:
    void printCmdLinePrefix();

    String getEnv(const String &key);
};

extern void initEnvs(int argc, char **argv);

extern void loopHandler();

#endif //TINY_SHELL_TINY_SHELL_H

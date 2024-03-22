//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_TINY_SHELL_H
#define TINY_SHELL_TINY_SHELL_H

#include "command.h"
#include "state.h"
#include "system_api.h"
#include "context.h"

class TinyShell
{
public:
    explicit TinyShell() : TinyShell(0, nullptr)
    {};

    explicit TinyShell(int argc, char **argv);

    int loopHandler();

private:
    void printCmdLinePrefix();

    String getCmdLine();

    void backSpace(size_t &cursorPos, String &cmdLine);

    void cursorMoveHead(size_t &cursorPos);

    void cursorMoveLeft(size_t &cursorPos, bool &convert);

    void cursorMoveRight(size_t &cursorPos, int cmdLineLen, bool &convert);

    void printChar(char ch, size_t &cursorPos, String &cmdLine);

    void clearPrefixCmdLine(size_t &cursorPos, String &cmdLine);

    void showPreCmd(size_t &curHistoryCmdPos, size_t &cursorPos, String &cmdLine, bool &convert);

    void clearCmdLine(size_t &cursorPos, String &cmdLine);

    void showNextCmd(size_t &curHistoryCmdPos, size_t &cursorPos, String &cmdLine, bool &convert);

    static void printOutput(int output);

    void myPipe(const Vector<String> &cmd, int &input, int &output, pid_t &childPid);
};

extern void initEnvs(int argc, char **argv);

extern void loopHandler();

#endif //TINY_SHELL_TINY_SHELL_H

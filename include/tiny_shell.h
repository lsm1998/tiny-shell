//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_TINY_SHELL_H
#define TINY_SHELL_TINY_SHELL_H

#include "command.h"
#include "state.h"
#include "system_api.h"

typedef struct HistoryCmd
{
    String cmdLine;
    size_t index;
    String execTime;  // 执行时间
} HistoryCmd;

enum ParserStatus
{
    INIT = 1,  // 初始状态
    PIPE = 2,  // 管道
    WORD = 3,  // 单词
};

class TinyShell
{
private:
    std::unordered_map<String, String> envs = {};  // 环境变量列表
    struct termios oldAttr{}, newAttr{};  // 终端属性
    size_t historyCmdPos = 0;  // 当前历史命令在historyCmdLines的下标
    Vector<HistoryCmd> historyCmdLines;  // 用于保存历史命令

public:
    explicit TinyShell() : TinyShell(0, nullptr)
    {};

    explicit TinyShell(int argc, char **argv);

    int loopHandler();

private:
    void printCmdLinePrefix();

    String getEnv(const String &key);

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

    void setHistory(const String &cmdLine);

    void parser(String &cmdLine, Vector<Vector<String>> &cmd);

    void parserToken(String &cmdLine, Vector<String> &tokens);

    void parserCmd(Vector<String> &tokens, Vector<Vector<String>> &cmd);

    void execCmd(Vector<Vector<String>> &cmd);

    void execSingleCmd(Vector<String> &cmd);

    void execPipeCmd(Vector<Vector<String>> &cmd);

    void printOutput(int output);

    void env();

    void cd(Vector<String> &cmd);

    void updatePwd(String newPwd);

    void pwd(Vector<String> &cmd);

    void history(Vector<String> &cmd);

    void execExternalCmd(Vector<String> &cmd);

    String getCommand(const Vector<String> &cmd);

    void myPipe(const Vector<String> &cmd, int &input, int &output, pid_t &childPid);
};

extern void initEnvs(int argc, char **argv);

extern void loopHandler();

#endif //TINY_SHELL_TINY_SHELL_H

//
// Created by Administrator on 2024/3/20.
//

#include "command.h"
#include "exit.cpp"
#include "parse.h"

namespace tinyShell
{
    Vector<BaseCommand> historyCmdLines = {};

    void parserToken(String &cmdLine, Vector<String> &tokens)
    {
        int32_t status = INIT;
        String token;
        auto addToken = [&token, &tokens]()
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token = "";
            }
        };
        for (const auto &c: cmdLine)
        {  // 通过有限状态机来解析命令行
            if (status == INIT)
            {
                if (isblank(c))
                {
                    continue;
                }
                if (c == '|')
                {
                    status = PIPE;
                } else
                {
                    status = WORD;
                }
                token.push_back(c);
            } else if (status == PIPE)
            {
                if (isblank(c))
                {
                    addToken();
                    status = INIT;
                } else if (c == '|')
                {
                    token.push_back(c);
                    status = WORD;
                } else
                {
                    addToken();
                    token.push_back(c);
                    status = WORD;
                }
            } else
            {
                if (isblank(c))
                {
                    status = INIT;
                    addToken();
                } else if (c == '|')
                {
                    status = PIPE;
                    addToken();
                    token.push_back(c);
                } else
                {
                    token.push_back(c);  // 还是维持在WORD的状态
                }
            }
        }
        addToken();
    }

    void parserCmd(Vector<String> &tokens, Vector<Vector<String>> &cmd)
    {
        Vector<String> oneCmd;
        tokens.emplace_back("|");  // tokens最后固定添加一个管道标识，方便后面的解析
        for (const auto &token: tokens)
        {
            if (token != "|")
            {
                oneCmd.push_back(token);
                continue;
            }
            if (!oneCmd.empty())
            {
                cmd.push_back(oneCmd);
                oneCmd.clear();
            }
        }
    }

    void parser(String &cmdLine, Vector<Vector<String>> &cmd)
    {
        Vector<String> tokens;
        parserToken(cmdLine, tokens);
        parserCmd(tokens, cmd);
    }

    Vector<Vector<String>> parseCommandString(const String &cmdLine)
    {
        Vector<Vector<String>> cmd;
        parser(const_cast<String &>(cmdLine), cmd);
        return cmd;
    }

    Command *parseCommand(const Vector<Vector<String>> &commands)
    {
        if (commands.size() == 2)
        {
            return pipeCmd;
        }
        String cmdName = commands[0][0];

        for (auto &item: commandList)
        {
            if (cmdName == item->name())
            {
                return item;
            }
        }
        return singleExternalCmd;
    }

    Command::~Command()
    {
        // 设置历史命令
        if (cmdLine.empty()) return; // 空命令不记录
        struct timeval curTime{};
        tm localTime;
        char temp[100] = {0};
#if (defined LINUX_OS) | (defined MAC_OS)
        gettimeofday(&curTime, nullptr);
        strftime(temp, 99, "%F %T", localtime(&curTime.tv_sec));
#elif (defined WINDOWS_OS)
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        // 将 SYSTEMTIME 转换为 tm 结构体
        tm lt{};
        localTime.tm_sec = sysTime.wSecond;
        localTime.tm_min = sysTime.wMinute;
        localTime.tm_hour = sysTime.wHour;
        localTime.tm_mday = sysTime.wDay;
        localTime.tm_mon = sysTime.wMonth - 1; // tm_mon 是从0开始的，而 wMonth 是从1开始的
        localTime.tm_year = sysTime.wYear - 1900; // tm_year 是从1900年开始计算的
        localTime.tm_wday = sysTime.wDayOfWeek;
        localTime.tm_yday = 0;
        localTime.tm_isdst = -1;
        strftime(temp, sizeof(temp), "%F %T", &lt);
#endif
        historyCmdLines.push_back(BaseCommand{cmdLine, index, execTime});
    }
}
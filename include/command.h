//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_COMMAND_H
#define TINY_SHELL_COMMAND_H

#include <unordered_map>
#include <map>
#include <cstring>
#include <unistd.h>
#include "define.h"
#include "context.h"

namespace tinyShell
{
    enum ParserStatus
    {
        INIT = 1,  // 初始状态
        PIPE = 2,  // 管道
        WORD = 3,  // 单词
    };

    class BaseCommand
    {
    public:
        String cmdLine;
        size_t index;
        String execTime;  // 执行时间
    };

    class Command : public BaseCommand
    {
    public:
        virtual void execute(Vector<Vector<String>> args) = 0;

        [[nodiscard]] virtual String name() const = 0;

        virtual ~Command();

        static void printOutput(int output)
        {
            char c;
#if (defined  LINUX_OS) || (defined  MAC_OS)
            while (read(output, &c, 1) > 0)
            {
                std::cout << c;
            }
#elif defined(WINDOWS_OS)
            DWORD bytesRead;
            while (ReadFile((void *) output, &c, 1, &bytesRead, NULL) && bytesRead > 0)
            {
                std::cout << c;
            }
#endif
        }

        static String getCommand(const Vector<String> &cmd)
        {
            String command;
            for (const auto &i: cmd)
            {
                command = command.append(i).append(" ");
            }
            return command;
        }
    };
}

#endif //TINY_SHELL_COMMAND_H

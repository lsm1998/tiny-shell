//
// Created by Administrator on 2024/3/20.
//

#include "command.h"
#include "exit.cpp"
#include "parse.h"

namespace tinyShell
{
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

    Command::~Command() = default;
}
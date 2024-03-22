//
// Created by Administrator on 2024/3/22.
//

#include "command.h"
#include <cstring>

class CdCommand : public Command
{
public:
    void execute(Vector<Vector<String>> args) override
    {
        auto context = TinyShellContext::getInstance();
        Vector<String> cmd = args[0];
        String path;
        if (cmd.size() == 1)
        {
            // cd不带参数则跳转到home目录
            path = context->getEnv("HOME");
        } else
        {
            path = cmd[1];
        }
        if (path == ".")
        {  // 是当前目录，不用处理
            return;
        }
        if (path == "~")
        {
            // 跳转到home目录
            path = context->getEnv("HOME");
        } else if (path == "-")
        {  // 返回之前的旧目录
            path = context->getEnv("OLDPWD");
            std::cout << path << std::endl;  // 先打印出要跳转的目录
        }
        int ret = chdir(path.c_str());
        if (ret)
        {
            std::cout << SHELL_NAME << ": cd: " << path << ": " << strerror(errno) << std::endl;
            return;
        }
        context->updatePwd(path);  // 更新PWD这个环境变量
    }

    [[nodiscard]] String name() const override
    {
        return "cd";
    }
};
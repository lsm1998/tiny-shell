//
// Created by Administrator on 2024/3/17.
//

#include <iostream>
#include <tiny_shell.h>
#include <cstring>

TinyShell::TinyShell(int argc, char **argv)
{
    while (argv && argv[++argc])
    {
        // 命令行参数之后就是环境变量
        String item = argv[argc];
        size_t pos = item.find('=');
        if (pos != String::npos)
        {
            this->envs[item.substr(0, pos)] = item.substr(pos + 1);
        }
    }
    this->envs["SHELL"] = SHELL_NAME;
    // 调用setenv直接更新SHELL这个环境变量
    setenv("SHELL", SHELL_NAME, 1);
    tcgetattr(STDIN_FILENO, &this->oldAttr);  // 获取终端属性
    this->newAttr = this->oldAttr;
    this->newAttr.c_lflag &= ~(ICANON | ECHO);  // 关闭标准输入模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &this->newAttr);  // 设置终端属性
}

int TinyShell::loopHandler()
{
    String cmdLine = "";
    while (true)
    {
        this->printCmdLinePrefix();
        break;
    }
    return 0;
}

void TinyShell::printCmdLinePrefix()
{

    String pwd = SystemApi::getPath();
    std::cout << pwd << std::endl;
    String user = getEnv("USER");
    String hostName = getEnv("HOSTNAME");
    String base;
    if (pwd == getEnv("HOME"))
    {
        // home目录，则显示~
        base = "~";
    } else
    {
        base = basename((char *) pwd.c_str());
    }
    if (hostName.empty())
    {
        hostName = SHELL_NAME;
    }
    std::cout << user << "@" << hostName << " "
              << "\033[32m" << base << "\033[0m"
              << " $ ";
}

String TinyShell::getEnv(const String &key)
{
    return this->envs[key];
}

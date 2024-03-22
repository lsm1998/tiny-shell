//
// Created by Administrator on 2024/3/17.
//

#include <iostream>
#include "tiny_shell.h"
#include "parse.h"
#include "context.h"

TinyShell::TinyShell(int argc, char **argv)
{
    auto context = TinyShellContext::getInstance();
    while (argv && argv[++argc])
    {
        // 命令行参数之后就是环境变量
        String item = argv[argc];
        size_t pos = item.find('=');
        if (pos != String::npos)
        {
            context->setEnv(item.substr(0, pos), item.substr(pos + 1));
        }
    }
    context->setEnv("SHELL", SHELL_NAME);
    // 调用setenv直接更新SHELL这个环境变量
    setenv("SHELL", SHELL_NAME, 1);

    auto attr = context->getAttr();
    auto oldAttr = std::get<0>(attr);
    tcgetattr(STDIN_FILENO, oldAttr);  // 获取终端属性

    auto newAttr = std::get<0>(attr);
    newAttr->c_lflag &= ~(ICANON | ECHO);  // 关闭标准输入模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, newAttr);  // 设置终端属性
}

int TinyShell::loopHandler()
{
    while (true)
    {
        this->printCmdLinePrefix();
        String cmdLine = this->getCmdLine();  // 获取用户输入的命令行
        auto commands = parseCommandString(cmdLine);
        if (commands.empty() || commands[0].empty())
        {
            continue;
        }
        auto *command = parseCommand(commands);
        if (!command)
        {
            std::cout << SHELL_NAME << ": command not found: " << cmdLine << std::endl;
            continue;
        }
        // 执行命令行
        command->execute(commands);
    }
    return 0;
}

void TinyShell::printCmdLinePrefix()
{
    auto context = TinyShellContext::getInstance();
    String pwd = SystemApi::getPath();
    std::cout << pwd << std::endl;
    String user = context->getEnv("USER");
    String hostName = context->getEnv("HOSTNAME");
    String base;
    if (pwd == context->getEnv("HOME"))
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

String TinyShell::getCmdLine()
{
    auto context = TinyShellContext::getInstance();
    char ch;
    String cmdLine;
    constexpr char kBackspace = 127;  // 回退键
    constexpr char kEsc = 27;  // 转义序列的标识：kEsc
    constexpr char kCtrlA = 1;  // 【Ctrl + a】-> 输入光标移动到行首
    constexpr char kCtrlU = 21;  // 【Ctrl + u】-> 清空光标前的输入
    bool convert = false;  // 是否进入转义字符
    size_t cursorPos = 0;  // 光标位置，初始化为0
    size_t curHistoryCmdPos = context->getHistoryCmdLines().size();  // 当前历史命令的位置
    while (true)
    {
        ch = (char) getchar();
        if (ch == kEsc)
        {
            convert = true;
            continue;
        }
        if (ch == kBackspace)
        {
            backSpace(cursorPos, cmdLine);
            continue;
        }
        if (ch == kCtrlA)
        {
            cursorMoveHead(cursorPos);
            continue;
        }
        if (ch == kCtrlU)
        {
            clearPrefixCmdLine(cursorPos, cmdLine);
            continue;
        }
        if (ch == '\n')
        {
            putchar(ch);
            return cmdLine;
        }
        if (convert && ch == 'A')
        {  // 上移光标-上一条历史命令
            showPreCmd(curHistoryCmdPos, cursorPos, cmdLine, convert);
            continue;
        }
        if (convert && ch == 'B')
        {  // 下移光标-下一条历史命令
            showNextCmd(curHistoryCmdPos, cursorPos, cmdLine, convert);
            continue;
        }
        if (convert && ch == 'C')
        {  // 右移光标
            cursorMoveRight(cursorPos, cmdLine.size(), convert);
            continue;
        }
        if (convert && ch == 'D')
        {  // 左移光标
            cursorMoveLeft(cursorPos, convert);
            continue;
        }
        if (convert && ch == '[')
        {  // 转义字符，则不打印'['
            continue;
        }
        if (isprint(ch))
        {
            printChar(ch, cursorPos, cmdLine);
        }
    }
}

void TinyShell::backSpace(size_t &cursorPos, String &cmdLine)
{
    if (cursorPos <= 0) return;  // 光标已经到最左边了
    if (cursorPos == cmdLine.size())
    {  //光标在输入的最后
        cursorPos--;
        printf("\b \b");
        cmdLine = cmdLine.substr(0, cmdLine.size() - 1);
        return;
    }
    // 执行到这里，说明是在光标在输入的中间，需要删除光标前面的一个字符，并把后面的字段都向前移动一格
    String tail = cmdLine.substr(cursorPos);
    cursorPos--;
    printf("\b");  // 退一格
    for (size_t i = cursorPos; i < cmdLine.size(); i++)
    {  // 抹掉后面的输出
        printf(" ");
    }
    for (size_t i = cursorPos; i < cmdLine.size(); i++)
    {  // 光标回退
        printf("\b");
    }
    printf("%s", tail.c_str());  // 这个时候打印，实现"后面的字段都向前移动一格"
    for (size_t i = 0; i < tail.size(); i++)
    {  // 光标再回退
        printf("\b");
    }
    cmdLine.erase(cursorPos, 1);  // 删除cmdLine中的字符
}

void TinyShell::cursorMoveHead(size_t &cursorPos)
{
    for (int i = 0; i < cursorPos; i++)
    {
        printf("\033[1D");
    }
    cursorPos = 0;
}

void TinyShell::cursorMoveLeft(size_t &cursorPos, bool &convert)
{
    if (cursorPos > 0)
    {
        printf("\033[1D");  // 光标左移一格的组合
        cursorPos--;
    }
    convert = false;
}

void TinyShell::cursorMoveRight(size_t &cursorPos, int cmdLineLen, bool &convert)
{
    if (cursorPos < cmdLineLen)
    {
        printf("\033[1C");  // 光标右移一格的组合
        cursorPos++;
    }
    convert = false;
}

void TinyShell::printChar(char ch, size_t &cursorPos, String &cmdLine)
{
    if (cursorPos == cmdLine.size())
    {  // 光标在输入的尾部，则把字符插入尾部
        putchar(ch);
        cursorPos++;
        cmdLine += ch;
        return;
    }
    // 执行到这里表示光标在输入的中间，除了输出当前字符之外，还需要把后面的字符往后移动一格
    String tail = cmdLine.substr(cursorPos);
    cmdLine.insert(cursorPos, 1, ch);
    cursorPos++;
    printf("%c%s", ch, tail.c_str());
    for (size_t i = 0; i < tail.size(); i++)
    {  // 光标需要退回到插入的位置
        printf("\033[1D");
    }
}

void TinyShell::clearPrefixCmdLine(size_t &cursorPos, String &cmdLine)
{
    for (int i = 0; i < cursorPos; i++)
    {  // 光标移动到输入起点
        printf("\b");
    }
    for (size_t i = 0; i < cmdLine.size(); i++)
    {  // 清空输入的内容
        printf(" ");
    }
    for (size_t i = 0; i < cmdLine.size(); i++)
    {  // 光标退回输入起点
        printf("\b");
    }
    cmdLine = cmdLine.substr(cursorPos);
    printf("%s", cmdLine.c_str());
    for (size_t i = 0; i < cmdLine.size(); i++)
    {  // 光标退回输入起点
        printf("\b");
    }
    cursorPos = 0;
}

void TinyShell::showPreCmd(size_t &curHistoryCmdPos, size_t &cursorPos, String &cmdLine, bool &convert)
{
    auto context = TinyShellContext::getInstance();
    if (!context->getHistoryCmdLines().empty() && curHistoryCmdPos > 0)
    {
        // 有历史命令才处理
        this->clearCmdLine(cursorPos, cmdLine);
        curHistoryCmdPos--;
        cmdLine = context->getHistoryCmdLines()[curHistoryCmdPos].cmdLine;
        cursorPos = cmdLine.size();
        printf("%s", cmdLine.c_str());  // 打印被选择的历史命令行
    }
    convert = false;
}

void TinyShell::clearCmdLine(size_t &cursorPos, String &cmdLine)
{
    for (size_t i = cursorPos; i < cmdLine.size(); i++)
    {  // 清空光标后面的内容
        printf(" ");
    }
    for (size_t i = cursorPos; i < cmdLine.size(); i++)
    {  // 光标回退
        printf("\b");
    }
    while (cursorPos > 0)
    {  // 清空终端当前行打印的内容
        // 通过光标回退一格，然后打印空白符，最后再回退一格的方来实现命令行输入中最后一个字符的清除
        printf("\b \b");
        cursorPos--;
    }
    cmdLine = "";  // 清空命令行
}

void TinyShell::showNextCmd(size_t &curHistoryCmdPos, size_t &cursorPos, String &cmdLine, bool &convert)
{
    auto context = TinyShellContext::getInstance();
    if (!context->getHistoryCmdLines().empty() && curHistoryCmdPos < context->getHistoryCmdLines().size() - 1)
    {  // 有历史命令才处理
        clearCmdLine(cursorPos, cmdLine);
        curHistoryCmdPos++;
        cmdLine = context->getHistoryCmdLines()[curHistoryCmdPos].cmdLine;
        cursorPos = cmdLine.size();
        printf("%s", cmdLine.c_str());  // 打印被选择的历史命令行
    }
    convert = false;
}

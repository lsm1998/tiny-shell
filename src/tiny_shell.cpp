//
// Created by Administrator on 2024/3/17.
//

#include <iostream>
#include <tiny_shell.h>
#include <cstring>
#include <utility>
#include <sys/time.h>
#include <sys/wait.h>

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
    while (true)
    {
        this->printCmdLinePrefix();
        String cmdLine = this->getCmdLine();  // 获取用户输入的命令行
        if (cmdLine == "exit")
        {
            break;
        }
        Vector<Vector<String>> cmd;
        this->setHistory(cmdLine);  // 设置历史执行命令
        this->parser(cmdLine, cmd);  // 解析命令行
        this->execCmd(cmd);  // 执行命令行
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

String TinyShell::getCmdLine()
{
    char ch;
    String cmdLine;
    constexpr char kBackspace = 127;  // 回退键
    constexpr char kEsc = 27;  // 转义序列的标识：kEsc
    constexpr char kCtrlA = 1;  // 【Ctrl + a】-> 输入光标移动到行首
    constexpr char kCtrlU = 21;  // 【Ctrl + u】-> 清空光标前的输入
    bool convert = false;  // 是否进入转义字符
    size_t cursorPos = 0;  // 光标位置，初始化为0
    size_t curHistoryCmdPos = historyCmdLines.size();  // 当前历史命令的位置
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
    if (!historyCmdLines.empty() && curHistoryCmdPos > 0)
    {
        // 有历史命令才处理
        this->clearCmdLine(cursorPos, cmdLine);
        curHistoryCmdPos--;
        cmdLine = historyCmdLines[curHistoryCmdPos].cmdLine;
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
    if (!historyCmdLines.empty() && curHistoryCmdPos < historyCmdLines.size() - 1)
    {  // 有历史命令才处理
        clearCmdLine(cursorPos, cmdLine);
        curHistoryCmdPos++;
        cmdLine = historyCmdLines[curHistoryCmdPos].cmdLine;
        cursorPos = cmdLine.size();
        printf("%s", cmdLine.c_str());  // 打印被选择的历史命令行
    }
    convert = false;
}

void TinyShell::setHistory(const String &cmdLine)
{
    if (cmdLine.empty()) return;  // 空命令不记录
    struct timeval curTime{};
    char temp[100] = {0};
    gettimeofday(&curTime, nullptr);
    strftime(temp, 99, "%F %T", localtime(&curTime.tv_sec));
    HistoryCmd historyCmd;
    historyCmd.index = historyCmdPos++;
    historyCmd.execTime = temp;
    historyCmd.cmdLine = cmdLine;
    historyCmdLines.push_back(historyCmd);
}

void TinyShell::parser(String &cmdLine, Vector<Vector<String>> &cmd)
{
    Vector<String> tokens;
    this->parserToken(cmdLine, tokens);
    this->parserCmd(tokens, cmd);
}

void TinyShell::parserToken(String &cmdLine, Vector<String> &tokens)
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

void TinyShell::parserCmd(Vector<String> &tokens, Vector<Vector<String>> &cmd)
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

void TinyShell::execCmd(Vector<Vector<String>> &cmd)
{
    if (cmd.empty()) return;
    if (cmd.size() == 1)
    {
        execSingleCmd(cmd[0]);
    } else
    {
        execPipeCmd(cmd);
    }
}

void TinyShell::execSingleCmd(Vector<String> &cmd)
{
    if (cmd[0] == "exit")
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldAttr);  // 恢复终端属性
        exit(0);  // exit的内置命令，则直接退出
    } else if (cmd[0] == "env")
    {
        env();
    } else if (cmd[0] == "cd")
    {
        cd(cmd);
    } else if (cmd[0] == "pwd")
    {
        pwd(cmd);
    } else if (cmd[0] == "history")
    {
        history(cmd);
    } else
    {
        execExternalCmd(cmd);  // 执行外部命令
    }
}

void TinyShell::execPipeCmd(Vector<Vector<String>> &cmd)
{
    int input = -1;
    int output = -1;
    int status = 0;
    pid_t childPid = 0;
    bool result = true;
    for (const auto &oneCmd: cmd)
    {
        myPipe(oneCmd, input, output, childPid);
        // 调用waitpid等待子进程执行子命令结束，并获取子命令的执行结果
        int ret = waitpid(childPid, &status, 0);
        // waitpid调用成功时，返回的是结束子进程的进程id
        if (ret != childPid)
        {
            std::cout << SHELL_NAME << ": " << oneCmd[0] << ": " << strerror(errno) << std::endl;
            result = false;
            break;
        }
        // 判断子命令执行结果，是否是正常退出且退出码是否为0
        // WIFEXITED用于判断子命令是否是正常退出，WEXITSTATUS用于判断正常退出的退出码
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            result = false;
            break;
        }
        // 这里很关键，使用子命令的标准输出数据关联的文件描述符output给input赋值
        // 在执行下一个子命令时，子命令的标准输入会重定向到input
        // 即实现了把上一个子命令的标准输出传递给下一个子命令标准输入的功能
        input = output;
    }
    if (result) printOutput(output);
}

void TinyShell::printOutput(int output)
{
    char c;
    while (read(output, &c, 1) > 0)
    {
        std::cout << c;
    }
}

void TinyShell::env()
{
    for (const auto &item: envs)
    {
        std::cout << item.first << "=" << item.second << std::endl;
    }
}

void TinyShell::cd(Vector<String> &cmd)
{
    String path;
    if (cmd.size() == 1)
    {  // cd不带参数则跳转到home目录
        path = getEnv("HOME");
    } else
    {
        path = cmd[1];
    }
    if (path == ".")
    {  // 是当前目录，不用处理
        return;
    }
    if (path == "~")
    {  // 跳转到home目录
        path = getEnv("HOME");
    } else if (path == "-")
    {  // 返回之前的旧目录
        path = getEnv("OLDPWD");
        std::cout << path << std::endl;  // 先打印出要跳转的目录
    }
    int ret = chdir(path.c_str());
    if (ret)
    {
        std::cout << SHELL_NAME << ": cd: " << path << ": " << strerror(errno) << std::endl;
        return;
    }
    this->updatePwd(path);  // 更新PWD这个环境变量
}

void TinyShell::updatePwd(String newPwd)
{
    envs["OLDPWD"] = envs["PWD"];
    envs["PWD"] = std::move(newPwd);
}

void TinyShell::pwd(Vector<String> &cmd)
{
    std::cout << SystemApi::getPath() << std::endl;
}

void TinyShell::history(Vector<String> &cmd)
{
    for (const auto &item: historyCmdLines)
    {
        std::cout << item.index << " " << item.execTime << " " << item.cmdLine << std::endl;
    }
}

void TinyShell::execExternalCmd(Vector<String> &cmd)
{
    String file = cmd[0];
    pid_t pid = fork();
    if (pid < 0)
    {
        std::cout << SHELL_NAME << ": " << file << ": " << strerror(errno) << std::endl;
        return;
    }
    if (pid == 0)
    {  // 子进程，使用子进程执行外部命令
        // 调用execl使用bash来执行单独的子命令，第一个参数是bash程序的绝对路径，相当于执行"bash -c command"
        // execl执行之后就陷入了bash命令中，bash命令执行失败时才会返回
        execl("/bin/bash", "bash", "-c", getCommand(cmd).c_str(), nullptr);
        exit(1);  // bash命令执行失败，则直接调用exit退出
    }
    int status = 0;
    int ret = waitpid(pid, &status, 0);  // 父进程调用waitpid等待子进程执行子命令结束，并获取子命令的执行结果
    if (ret != pid)
    {
        std::cout << SHELL_NAME << ": " << file << ": " << strerror(errno) << std::endl;
    }
}

String TinyShell::getCommand(const Vector<String> &cmd)
{
    String command;
    for (const auto &i: cmd)
    {
        command = command.append(i).append(" ");
    }
    return command;
}

void TinyShell::myPipe(const Vector<String> &cmd, int &input, int &output, pid_t &childPid)
{
    int pfd[2];
    if (pipe(pfd) < 0)
    {  // 用于创建匿名管道的两个文件描述符，pfd[0]用于读，pfd[1]用于写。
        return;
    }
    pid_t pid = fork();  // 调用fork来创建子进程
    if (pid < 0)
    {
        return;
    }
    if (0 == pid)
    {  // 子进程
        if (input != -1)
        {
            dup2(input, STDIN_FILENO);  // 重定向标准输入
            close(input);
        }
        dup2(pfd[1], STDOUT_FILENO);  // 重定向标准输出
        close(pfd[0]);
        close(pfd[1]);
        execl("/bin/bash", "bash", "-c", getCommand(cmd).c_str(), (char *) 0);
        exit(1);
    }
    // 执行到这里是父进程
    close(pfd[1]);
    childPid = pid;  // 返回子进程的进程id
    output = pfd[0];  // 返回父进程管道的"读文件描述符"，从这个文件描述符中可以获取子命令的标准输出
}

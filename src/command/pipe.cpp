//
// Created by Administrator on 2024/3/22.
//

#include "command.h"

namespace tinyShell
{
    class PipeCommand : public Command
    {
    public:
        void execute(Vector<Vector<String>> args) override
        {
            int input = -1;
            int output = -1;
            int status = 0;
            pid_t childPid = 0;
            bool result = true;
            for (const auto &oneCmd: args)
            {
                execPipe(oneCmd, input, output, childPid);
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

        void execPipe(const Vector<String> &cmd, int &input, int &output, pid_t &childPid)
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

        [[nodiscard]] String name() const override
        {
            return "";
        }
    };
}
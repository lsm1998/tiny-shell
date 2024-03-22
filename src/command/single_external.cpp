//
// Created by Administrator on 2024/3/22.
//

#include "command.h"

class SingleExternalCommand : public Command
{
public:
    void execute(Vector<Vector<String>> args) override
    {
        Vector<String> cmd = args[0];

        String file = cmd[0];
        pid_t pid = fork();
        if (pid < 0)
        {
            std::cout << SHELL_NAME << ": " << file << ": " << strerror(errno) << std::endl;
            return;
        }
        if (pid == 0)
        {
            // 使用子进程执行外部命令
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

    [[nodiscard]] String name() const override
    {
        return "";
    }


};
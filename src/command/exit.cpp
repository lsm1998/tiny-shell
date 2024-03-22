//
// Created by Administrator on 2024/3/20.
//

#include "command.h"

namespace tinyShell
{
    class ExitCommand : public Command
    {
    public:
        void execute(Vector<Vector<String>> args) override
        {
            auto attr = TinyShellContext::getInstance()->getAttr();
            // 恢复终端属性
#if (defined  LINUX_OS) || (defined  MAC_OS)
            tcsetattr(STDIN_FILENO, TCSANOW, std::get<0>(attr));
#elif (defined WINDOWS_OS)
            SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), std::get<0>(attr)->c_cflag);
#endif
            exit(0);
        }

        [[nodiscard]] String name() const override
        {
            return "exit";
        }
    };
}
//
// Created by Administrator on 2024/3/20.
//

#include "command.h"

class ExitCommand : public Command
{
public:
    void execute(Vector<Vector<String>> args) override
    {
        auto attr = TinyShellContext::getInstance()->getAttr();
        // 恢复终端属性
        tcsetattr(STDIN_FILENO, TCSANOW, std::get<0>(attr));
        exit(0);
    }

    [[nodiscard]] String name() const override
    {
        return "exit";
    }
};
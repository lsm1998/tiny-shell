//
// Created by Administrator on 2024/3/22.
//

#include "command.h"

class HistoryCommand : public Command
{
public:
    void execute(Vector<Vector<String>> args) override
    {
        for (const auto &item: TinyShellContext::getInstance()->getHistoryCmdLines())
        {
            std::cout << item.index << " " << item.execTime << " " << item.cmdLine << std::endl;
        }
    }

    [[nodiscard]] String name() const override
    {
        return "history";
    }
};
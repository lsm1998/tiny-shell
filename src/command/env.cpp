//
// Created by Administrator on 2024/3/22.
//

#include "command.h"

namespace tinyShell
{
    class EnvCommand : public Command
    {
    public:
        void execute(Vector<Vector<String>> args) override
        {
            for (const auto &item: TinyShellContext::getInstance()->getEnvs())
            {
                std::cout << item.first << "=" << item.second << std::endl;
            }
        }

        [[nodiscard]] String name() const override
        {
            return "env";
        }
    };
}
//
// Created by Administrator on 2024/3/22.
//

#include "command.h"
#include "system_api.h"

namespace tinyShell
{
    class PwdCommand : public Command
    {
    public:
        void execute(Vector<Vector<String>> args) override
        {
            std::cout << SystemApi::getPath() << std::endl;
        }

        [[nodiscard]] String name() const override
        {
            return "pwd";
        }
    };
}
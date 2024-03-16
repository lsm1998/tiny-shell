//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_COMMAND_H
#define TINY_SHELL_COMMAND_H

#include <string>
#include <unistd.h>
#include "define.h"

using String = std::string;

class Command
{
public:
    virtual void execute(String args...) = 0;

    virtual ~Command() = default;
};

#endif //TINY_SHELL_COMMAND_H

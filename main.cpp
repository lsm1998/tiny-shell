//
// Created by Administrator on 2024/3/17.
//

#include "tiny_shell.h"

int main(int argc, char **argv)
{
    using namespace tinyShell;
    TinyShell tinyShell(argc, argv);
    return tinyShell.loopHandler();
}
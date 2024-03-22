//
// Created by Administrator on 2024/3/22.
//

#ifndef TINY_SHELL_PARSE_H
#define TINY_SHELL_PARSE_H

#include "command.h"

namespace tinyShell
{
    extern Vector<Command *> commandList;
    extern Command *singleExternalCmd;
    extern Command *pipeCmd;

    extern Vector<Vector<String>> parseCommandString(const String &cmdLine);

    extern Command *parseCommand(const Vector<Vector<String>> &commands);

    extern void parserCmd(Vector<String> &tokens, Vector<Vector<String>> &cmd);

    extern void parserToken(String &cmdLine, Vector<String> &tokens);

    extern void parser(String &cmdLine, Vector<Vector<String>> &cmd);
}
#endif //TINY_SHELL_PARSE_H

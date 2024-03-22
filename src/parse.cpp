//
// Created by Administrator on 2024/3/22.
//

#include "parse.h"
#include "command/exit.cpp"
#include "command/cd.cpp"
#include "command/env.cpp"
#include "command/history.cpp"
#include "command/pwd.cpp"
#include "command/single_external.cpp"
#include "command/pipe.cpp"

Vector<Command *> commandList = {
        new ExitCommand,
        new CdCommand,
        new PwdCommand,
        new HistoryCommand,
        new CdCommand,
};

Command *singleExternalCmd = new SingleExternalCommand;
Command *pipeCmd = new PipeCommand;


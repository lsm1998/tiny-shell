//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_SYS_API_H
#define TINY_SHELL_SYS_API_H

#include <string>
#include "define.h"

namespace tinyShell
{
    class SystemApi
    {
    public:
        // 获取当前工作目录
        static String getPath();
    };
}

#endif //TINY_SHELL_SYS_API_H

//
// Created by Administrator on 2024/3/17.
//

#include "system_api.h"

#if (defined  LINUX_OS) || (defined  MAC_OS)
#include <unistd.h>
#elif defined WINDOWS_OS
#include <processenv.h>
#endif
namespace tinyShell
{
    String SystemApi::getPath()
    {
#if (defined  LINUX_OS) || (defined  MAC_OS)
        char path[2048]{0};
        getcwd(path, 2048);
        return path;
#elif defined WINDOWS_OS
        char path[2048]{0};
        GetCurrentDirectory(2048, path);
        return path;
#endif
    }
}
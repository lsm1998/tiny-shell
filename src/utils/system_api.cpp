//
// Created by Administrator on 2024/3/17.
//

#include "system_api.h"

#if (defined  LINUX_OS) || (defined  MAC_OS)
#include <unistd.h>
#elif WINDOWS_OS
#include <direct.h>
#endif

String SystemApi::getPath()
{
#if (defined  LINUX_OS) || (defined  MAC_OS)
    char path[2048]{0};
    getcwd(path, 2048);
    return path;
#elif WINDOWS_OS
#endif
}

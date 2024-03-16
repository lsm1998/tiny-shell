//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_DEFINE_H
#define TINY_SHELL_DEFINE_H

#ifdef __linux__
#define LINUX_OS
#elif __APPLE__
#define MAC_OS
#elif WIN32
#define WINDOWS_OS
#endif

// Shell name
constexpr char *SHELL_NAME = (char *) "tinyShell";

#endif //TINY_SHELL_DEFINE_H

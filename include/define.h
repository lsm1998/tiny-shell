//
// Created by Administrator on 2024/3/17.
//

#ifndef TINY_SHELL_DEFINE_H
#define TINY_SHELL_DEFINE_H

#include <string>
#include <vector>
#include <iostream>

#ifdef __linux__
#define LINUX_OS
#elif __APPLE__
#define MAC_OS
#elif WIN32
#define WINDOWS_OS
#endif

using String = std::string;
template<class T>
using Vector = std::vector<T>;
template<class K, class V>
using Tuple2 = std::tuple<K, V>;
using Termios = struct termios;

// shell name
constexpr char *SHELL_NAME = (char *) "tinyShell";

#endif //TINY_SHELL_DEFINE_H

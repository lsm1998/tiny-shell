//
// Created by Administrator on 2024/3/22.
//

#ifndef TINY_SHELL_CONTEXT_H
#define TINY_SHELL_CONTEXT_H

#include <cstdlib>
#include <unordered_map>
#include "define.h"
#include "command.h"

namespace tinyShell
{
    class BaseCommand;

    class TinyShellContext
    {
    private:
        Termios *newAttr{};
        Termios *oldAttr{};
        size_t historyCmdPos = 0;  // 当前历史命令在historyCmdLines的下标
        std::unordered_map<String, String> envs = {};  // 环境变量列表
        Vector<BaseCommand> historyCmdLines;

    private:
        TinyShellContext();

    public:
        TinyShellContext(const TinyShellContext &) = delete;

        TinyShellContext(const TinyShellContext &&) = delete;

        ~TinyShellContext();

        static TinyShellContext *getInstance()
        {
            static TinyShellContext singleton;
            static CG cg;
            return &singleton;
        }

        void setEnv(const String &key, const String &value);

        String getEnv(const String &key) const;

        void updatePwd(const String &pwd);

        Tuple2<Termios *, Termios *> getAttr() const;

        Vector<BaseCommand> getHistoryCmdLines() const;

        std::unordered_map<String, String> getEnvs() const;

        class CG
        {
        public:
            ~CG()
            {
                delete TinyShellContext::getInstance();
            }
        };
    };
}
#endif //TINY_SHELL_CONTEXT_H

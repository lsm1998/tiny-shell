//
// Created by Administrator on 2024/3/22.
//

#include "context.h"

namespace tinyShell
{
    void TinyShellContext::setEnv(const String &key, const String &value)
    {
        this->envs[key] = value;
    }

    String TinyShellContext::getEnv(const String &key) const
    {
        auto val = this->envs.find(key);
        if (val == this->envs.end())
        {
            return "";
        }
        return val->second;
    }

    void TinyShellContext::updatePwd(const String &pwd)
    {
        envs["OLDPWD"] = envs["PWD"];
        envs["PWD"] = pwd;
    }

    Tuple2<Termios *, Termios *> TinyShellContext::getAttr() const
    {
        return {this->oldAttr, this->newAttr};
    }

    Vector<BaseCommand> TinyShellContext::getHistoryCmdLines() const
    {
        return this->historyCmdLines;
    }

    std::unordered_map <String, String> TinyShellContext::getEnvs() const
    {
        return this->envs;
    }

    TinyShellContext::TinyShellContext()
    {
        this->oldAttr = new Termios;
        this->newAttr = new Termios;
    }

    TinyShellContext::~TinyShellContext()
    {
        delete this->oldAttr;
        delete this->newAttr;
    }
}
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <map>
#include <ImGui-master/imgui.h>

class ConsoleBackend
{
    struct Command
    {
        std::string m_name;
        std::string m_hint;
        std::function<void(std::stringstream&)> m_wrapper;
    };

    std::map<std::string, Command> m_commands;
    std::vector<std::string> m_history;
    char m_inputBuf[256] = "";

public:
    template<typename... Args>
    void RegisterCommand(const std::string& name, const std::string& hint, std::function<void(Args...)> func)
    {
        m_commands[name] =
        {
            name,
            hint,
            [&, func](std::stringstream& ss)
            {
                std::tuple<Args...> parsedArgs;

                try
                {
                    std::apply([&](auto&... args)
                    {
                        ((args = parseArg<std::decay_t<decltype(args)>>(ss)), ...);
                    }, parsedArgs);

                    std::apply(func, parsedArgs);
                }
                catch (...)
                {
                    AddLog("[Error] " + name + ": Invalid arguments.");
                    std::cout << "Error: Invalid arguments for " << name << std::endl;
                }
            }
        };
    }

    void AddLog(std::string msg)
    {
        m_history.push_back(msg);
    }

    void Execute(const std::string& input)
    {
        std::stringstream ss(input);
        std::string cmdName;
        ss >> cmdName;

        auto it = m_commands.find(cmdName);
        if (it != m_commands.end())
        {
            it->second.m_wrapper(ss);
        }
        else
        {
            AddLog("Unknown command: " + cmdName);
            std::cout << "Unknown command: " << cmdName << std::endl;
        }
    }

private:
    template<typename T>
    T parseArg(std::stringstream& ss)
    {
        T val;

        if constexpr (std::is_same_v<T, std::string>)
        {
            ss >> std::ws;
            if (!(ss >> val)) throw std::runtime_error("String parse error");
        }
        else
        {
            if (!(ss >> val)) throw std::runtime_error("Parse error");
        }

        return val;
    }

    friend class Console;
};
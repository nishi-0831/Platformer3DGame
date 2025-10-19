#pragma once
#include <functional>
#include <typeindex>
#include <unordered_map>
#include "Command.h"
#include <concepts>

class CommandHandlerRegistry 
{
public:
    template<class CommandT>
    requires std::derived_from<CommandT, Command>
    void Register(std::function<void(CommandT&)> _execute, std::function<void(CommandT&)> _undo) 
    {
        const std::type_index key = typeid(CommandT);
        entries_[key] = 
        Entry
        {
            [func = std::move(_execute)](Command& _cmd) { func(static_cast<CommandT&>(_cmd)); },
            [func = std::move(_undo)](Command& _cmd) { func(static_cast<CommandT&>(_cmd)); }
        };
    }

    bool Execute(Command& _cmd) const 
    {
        if (auto itr = entries_.find(typeid(_cmd)); itr != entries_.end()) 
        {
            itr->second.execute(_cmd); 
            return true;
        }
        return false;
    }
    bool Undo(Command& _cmd) const 
    {
        if (auto itr = entries_.find(typeid(_cmd)); itr != entries_.end()) 
        {
            itr->second.undo(_cmd); 
            return true;
        }
        return false;
    }

private:
    struct Entry 
    {
        std::function<void(Command&)> execute;
        std::function<void(Command&)> undo;
    };
    std::unordered_map<std::type_index, Entry> entries_;
};
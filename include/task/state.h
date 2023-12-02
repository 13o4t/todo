#pragma once


#include <variant>

#include "task/task.h"

namespace task {

    struct StateBase {
        Task task;
    };
    struct StateStart : StateBase {};
    struct StateEnd : StateBase{};

    struct StateText : StateBase{};
    struct StateBlank : StateBase{};

    struct StateCompletion : StateBase {};

    struct StatePriority : StateBase {
        char priority;
    };
    struct StatePriorityEnd : StateBase {};

    struct StateProject : StateBase {
        std::string project;
    };

    struct StateContext : StateBase {
        std::string context;
    };

    struct StateSpecial : StateBase {
        std::string key;
        std::string value;
    };

    using States = std::variant<
        StateStart, StateEnd, StateText, StateBlank, StateCompletion, 
        StatePriority, StatePriorityEnd, StateProject, 
        StateContext, StateSpecial>;
}
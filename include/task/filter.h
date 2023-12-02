#pragma once

#include "task/task_export.h"
#include "task/task.h"

namespace task::filter {

    TASK_EXPORT bool has_project(const std::set<std::string>& projects, const Task& task);
    TASK_EXPORT bool has_context(const std::set<std::string>& contexts, const Task& task);

    TASK_EXPORT bool before_date(const std::string& date, const Task& task);
}
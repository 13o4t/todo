#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "task/task_export.h"

namespace task::date {

    const std::string FORMAT = "%Y-%m-%d";

    TASK_EXPORT bool valid(const std::string& date);

    TASK_EXPORT std::optional<std::string> convert(const std::string& date);

    TASK_EXPORT std::string today();
    TASK_EXPORT std::string tomorrow();

    TASK_EXPORT std::string sunday();
    TASK_EXPORT std::string monday();
    TASK_EXPORT std::string tuesday();
    TASK_EXPORT std::string wednesday();
    TASK_EXPORT std::string thursday();
    TASK_EXPORT std::string friday();
    TASK_EXPORT std::string saturday();

    TASK_EXPORT std::string month_last_day();
}
#pragma once

#include <optional>
#include <set>
#include <string>

#include "nlohmann/json.hpp"

#include "task/task_export.h"

namespace task {

    struct Config {
        std::string file;
        std::string archive_file;

        std::set<std::string> filter_projects;
        std::set<std::string> filter_contexts;
        std::optional<std::string> filter_due_date;
    };

    TASK_EXPORT void to_json(nlohmann::json& j, const Config& config);
    TASK_EXPORT void from_json(const nlohmann::json& j, Config& config);
}
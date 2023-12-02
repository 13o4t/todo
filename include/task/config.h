#pragma once

#include <optional>
#include <set>
#include <string>

namespace task {

    struct Config {
        std::string file;
        std::string archive_file;

        std::set<std::string> filter_projects;
        std::set<std::string> filter_contexts;
        std::optional<std::string> filter_due_date;
    };
}
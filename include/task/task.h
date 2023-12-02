#pragma once

#include <set>
#include <string>
#include <unordered_map>

namespace task {

    struct Task {
        std::size_t number{0};

        std::string text{""};

        bool is_completion{false};
        char priority{' '};

        std::string completion_date{""};
        std::string creation_date{""};

        std::set<std::string> projects{};
        std::set<std::string> contexts{};
        std::unordered_map<std::string, std::string> specials{};
    };
}
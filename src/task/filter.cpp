#include "task/filter.h"

#include <algorithm>

#include "spdlog/spdlog.h"

#include "task/date.h"

namespace task::filter {

    bool has_project(const std::set<std::string>& projects, const Task& task) {
        std::set<std::string> intersection;
        std::set_intersection(projects.begin(), projects.end(), task.projects.begin(), task.projects.end(), 
                            std::inserter(intersection, intersection.begin()));
        return !intersection.empty();
    }

    bool has_context(const std::set<std::string>& contexts, const Task& task) {
        std::set<std::string> intersection;
        std::set_intersection(contexts.begin(), contexts.end(), task.contexts.begin(), task.contexts.end(), 
                            std::inserter(intersection, intersection.begin()));
        return !intersection.empty();
    }

    bool before_date(const std::string& date, const Task& task) {
        auto due_itr = task.specials.find("due");
        if (due_itr == task.specials.end()) return false;
        std::string due = due_itr->second;

        spdlog::debug("filter before date {}: {} {}", date, due, due <= date);
        return due <= date;
    }
}
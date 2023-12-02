#pragma once

#include "spdlog/spdlog.h"

#include "task/date.h"
#include "task/manager.h"
#include "task/filter.h"

void output_file(const std::string& file) {
    task::Manager::instance().config().file = file;
}

void set_log_level(const int& level) {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

void set_filter(const std::vector<std::string>& filters) {
    for (auto& f : filters) {
        if (f.starts_with("+")) {
            task::Manager::instance().config().filter_projects.insert(f.substr(1));
        } else if (f.starts_with("@")) {
            task::Manager::instance().config().filter_contexts.insert(f.substr(1));
        } else if (f.starts_with("due:")) {
            std::string date = f.substr(4);
            auto valid_opt = task::date::convert(date);
            if (!valid_opt.has_value()) 
                throw std::invalid_argument(fmt::format("error due filter: {}", date));
            task::Manager::instance().config().filter_due_date = valid_opt.value();
        } else {
            throw std::invalid_argument(fmt::format("filter not support: {}", f));
        }
    }
}

void archive_file(const std::string& file) {
    task::Manager::instance().config().archive_file = file;
}
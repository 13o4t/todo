#pragma once

#include "spdlog/spdlog.h"

#include "task/manager.h"

void add_task(const std::string& description) {
    bool ok = task::Manager::instance().add_task(description);
    if (!ok) spdlog::critical("add todo task error: {}", description);
}

void done_task(const std::size_t number) {
    bool ok = task::Manager::instance().done_task(number);
    if (!ok) spdlog::critical("done todo task error: {}", number);
}

void delete_task(const std::size_t number) {
    bool ok = task::Manager::instance().delete_task(number);
    if (!ok) spdlog::critical("delete todo task error: {}", number);
}

void archive_task() {
    bool ok = task::Manager::instance().archive_task();
    if (!ok) spdlog::critical("archive todo task error");
}
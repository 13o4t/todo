#pragma once

#include "spdlog/spdlog.h"

#include "task/manager.h"

void list_tasks() {
    bool ok = task::Manager::instance().list_task();
    if (!ok) spdlog::critical("show todos error: {}");
}
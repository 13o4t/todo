#include "plugin_repeat.h"

#include <iostream>

#include "spdlog/spdlog.h"

#include "task/date.h"
#include "task/manager.h"

void after_done(const char* task_description) {
    task::Parser parser;
    auto res = parser.parse(task_description);
    if (!res.has_value()) return;
    auto task = std::move(res.value());

    if (!task.specials.contains("repeat")) return;

    auto due = task::date::convert(task.specials["repeat"]);
    if (!due.has_value()) return;

    std::size_t old_due_pos = task.text.find("due:");
    while (old_due_pos != std::string::npos) {
        std::size_t blank_pos = task.text.substr(old_due_pos).find(" ");
        if (blank_pos != std::string::npos)
            task.text = task.text.substr(0, old_due_pos) + task.text.substr(old_due_pos + blank_pos);
        else
            task.text = task.text.substr(0, old_due_pos);

        old_due_pos = task.text.find("due:");
    }

    task.text += " due:" + due.value();
    task.is_completion = false;
    task.creation_date = "";
    task.completion_date = "";

    std::string new_task = parser.to_string(task);
    spdlog::debug("[plugin:repeat] add new task: {}", new_task);

    task::Manager::instance().add_task(new_task);
}
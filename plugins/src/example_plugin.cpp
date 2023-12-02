#include "example_plugin.h"

#include <iostream>

#include "task/parser.h"

void after_done(const char* task_description) {
    task::Parser parser;
    auto res = parser.parse(task_description);
    if (!res.has_value()) return;
    auto task = std::move(res.value());

    std::cout << "task done: " << task.text << std::endl;
}
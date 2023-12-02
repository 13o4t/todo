#include <iostream>

#include "CLI/CLI.hpp"
#include "spdlog/spdlog.h"

#include "internal_use_only/include/build_info.hpp"

#include "command/config.hpp"
#include "command/manager.hpp"
#include "command/list.hpp"

int main(int argc, char** argv) {
    try {
        std::string description = fmt::format("v{}\n{}\n", build::version, build::description);

        CLI::App app{ description, std::string(build::name) };
        app.require_subcommand(1);

        app.add_option_function<std::string>("-f,--file", output_file, "Output filepath");
        app.add_option_function<int>("--log_level", set_log_level, "Set log level")
            ->check(CLI::Range(0, 6))->default_val(2)->run_callback_for_default();

        auto cmd_add = app.add_subcommand("add", "Add todo task");
        cmd_add->add_option_function<std::string>("description", add_task, "Task description")->required();

        auto cmd_done = app.add_subcommand("done", "Done todo task");
        cmd_done->add_option_function<std::size_t>("number", done_task, "Task number")->required();

        auto cmd_del = app.add_subcommand("del", "Delete todo task");
        cmd_del->add_option_function<std::size_t>("number", delete_task, "Task number")->required();

        auto cmd_list = app.add_subcommand("list", "List todos");
        cmd_list->add_option_function<std::vector<std::string>>("filter", set_filter, "Filters, like: +project, @context due:today");
        cmd_list->callback(list_tasks);

        auto cmd_archive = app.add_subcommand("archive", "Archive task");
        cmd_archive->add_option_function<std::string>("-o,--output", archive_file, "Archive filepath");
        cmd_archive->callback(archive_task);

        CLI11_PARSE(app, argc, argv);
    } catch (std::exception& e) {
        fmt::print(stderr, "fatal: {}\n", e.what());
    }
    return 0;
}
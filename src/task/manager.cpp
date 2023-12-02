#include "task/manager.h"

#include <fstream>
#include <iostream>
#include <list>

#include "spdlog/spdlog.h"

#include "task/date.h"
#include "task/filter.h"
#include "task/plugin.h"

namespace task {

    Manager::Manager() {
#ifdef _WIN32
        std::filesystem::path path(getenv("USERPROFILE"));
#else
        std::filesystem::path path(getenv("HOME"));
#endif
        path /= "todo.txt";
        cfg_.file = path.string();
    }

    Manager::~Manager() {}

    Config& Manager::config() { return cfg_; }

    bool Manager::add_task(const std::string& description) {
        spdlog::debug("start add task: {}", description);
        auto res = parser_.parse(description);
        if (!res.has_value()) {
            spdlog::error("parse task error");
            return false;
        }
        auto task = std::move(res.value());
        fill_task(task);

        std::ofstream file;
        file.open(cfg_.file, std::ios::out | std::ios::app);
        spdlog::debug("open out file {}: {}", cfg_.file, file.is_open());
        if (!file) return false;

        file << parser_.to_string(task) << std::endl;

        file.close();
        spdlog::debug("finish add task: {}", description);
        return true;
    }

    bool Manager::done_task(const std::size_t target_number) {
        spdlog::debug("start done task [{}]", target_number);

        auto res = begin_editing_file();
        if (!res.has_value()) return false;
        auto [in, out] = std::move(res.value());

        std::size_t number = 0;
        std::string line;
        while (std::getline(in, line)) {
            number++;
            spdlog::debug("getline [{}]: {}", number, line);

            if (number != target_number) {
                out << line << std::endl;
                continue;
            }

            auto res = parser_.parse(line);
            if (!res.has_value()) {
                out << line << std::endl;
                continue;
            }

            auto task = std::move(res.value());
            task.is_completion = true;
            fill_task(task);
            out << parser_.to_string(task) << std::endl;

            Plugin::instance().execute(Plugin::Type::kAfterDone, task);
        }

        if (!finish_editing_file(in, out)) return false;
        spdlog::debug("finish done task [{}]", target_number);
        return true;
    }

    bool Manager::delete_task(const std::size_t target_number) {
        spdlog::debug("start delete task [{}]", target_number);

        auto res = begin_editing_file();
        if (!res.has_value()) return false;
        auto [in, out] = std::move(res.value());

        std::size_t number = 0;
        std::string line;
        while (std::getline(in, line)) {
            number++;
            spdlog::debug("getline [{}]: {}", number, line);

            if (number != target_number) out << line << std::endl;
            else continue;
        }

        if (!finish_editing_file(in, out)) return false;
        spdlog::debug("finish delete task [{}]", target_number);
        return true;
    }

    bool Manager::archive_task() {
        spdlog::debug("start archive task");

        auto res = begin_editing_file();
        if (!res.has_value()) return false;
        auto [in, out] = std::move(res.value());

        std::ofstream archive;
        if (!cfg_.archive_file.empty()) {
            archive.open(cfg_.archive_file, std::ios::out | std::ios::app);
            spdlog::debug("open archive file {}: {}", cfg_.archive_file, archive.is_open());
        } else {
            archive.open(default_archive_filename(), std::ios::out | std::ios::app);
            spdlog::debug("open archive file {}: {}", default_archive_filename(), archive.is_open());
        }

        std::size_t number = 0;
        std::string line;
        while (std::getline(in, line)) {
            number++;
            spdlog::debug("getline [{}]: {}", number, line);

            if (line.starts_with("x ")) archive << line << std::endl;
            else out << line << std::endl;
        }

        archive.close();
        if (!finish_editing_file(in, out)) return false;
        spdlog::debug("finish archive task");
        return true;
    }

    bool Manager::list_task() {
        spdlog::debug("start list task");

        std::ifstream file;
        file.open(cfg_.file, std::ios::in);
        spdlog::debug("open in file {}: {}", cfg_.file, file.is_open());
        if (!file) return true;

        std::list<Task> tasks;

        std::size_t number = 0;
        std::string line;
        while (std::getline(file, line)) {
            number++;
            spdlog::debug("getline [{}]: {}", number, line);

            auto res = parser_.parse(line);
            if (!res.has_value()) continue;

            auto task = std::move(res.value());
            task.number = number;
            spdlog::debug("parse task [{}] success: {}", task.number, task.text);

            if (!check_filter(task)) continue;

            tasks.push_back(std::move(task));
        }

        file.close();

        tasks.sort([](const Task& a, const Task& b){
            if (a.priority == b.priority) return a.number < b.number;
            if (a.priority == ' ') return false;
            if (b.priority == ' ') return true;
            return a.priority < b.priority;
        });
        for (auto task : tasks) print_task(task);

        spdlog::debug("finish list task");
        return true;
    }

    std::string Manager::temp_filename() {
        return cfg_.file + ".tmp";
    }

    std::string Manager::backup_filename() {
        return cfg_.file + ".bak";
    }

    std::string Manager::default_archive_filename() {
        auto path = std::filesystem::path(cfg_.file).parent_path() / "archive.txt";
        return path.string();
    }

    std::optional<std::pair<std::ifstream, std::ofstream>> Manager::begin_editing_file() {
        std::ifstream in;
        in.open(cfg_.file, std::ios::in);
        spdlog::debug("open file {}: {}", cfg_.file, in.is_open());
        if (!in) return std::nullopt;

        std::ofstream out;
        out.open(temp_filename(), std::ios::out | std::ios::ate);
        spdlog::debug("open temp file {}: {}", temp_filename(), out.is_open());
        if (!out) return std::nullopt;

        return std::make_pair(std::move(in), std::move(out));
    }

    bool Manager::finish_editing_file(std::ifstream& in, std::ofstream& out) {
        in.close();
        out.close();

        std::error_code err;
        std::filesystem::rename(cfg_.file, backup_filename(), err);
        if (err) {
            spdlog::error("rename file {} to {}: {}", cfg_.file, backup_filename(), err.message());
            return false;
        }
        std::filesystem::rename(temp_filename(), cfg_.file, err);
        if (err) {
            spdlog::error("rename file {} to {}: {}", temp_filename(), cfg_.file, err.message());
            return false;
        }
        return true;
    }

    void replace_due_date(std::string& text, const std::string& date) {
        std::size_t start_pos = text.find("due:");
        if (start_pos == std::string::npos) return;
        start_pos += 4;

        std::size_t end_pos = text.find_first_of(" ", start_pos);
        if (end_pos == std::string::npos)
            text.replace(start_pos, end_pos - start_pos, date);
        else
            text.replace(start_pos, text.length() - start_pos, date);
    }

    void Manager::fill_task(Task& task) {
        if (task.is_completion && task.completion_date.empty()) task.completion_date = date::today();
        if (task.creation_date.empty()) task.creation_date = date::today();

        if (task.specials.contains("due")) {
            std::string date = task.specials["due"];
            auto valid_opt = date::convert(date);
            if (!valid_opt.has_value()) {
                throw std::invalid_argument(fmt::format("error due date: {}", date));
            }
            std::string valid_date = valid_opt.value();
            if (date != valid_date) {
                replace_due_date(task.text, valid_date);
                task.specials["due"] = valid_date;
            }
        }
    }

    bool Manager::check_filter(const Task& task) {
        bool ok = true;

        if (ok) ok &= !task.is_completion;

        if (ok && !cfg_.filter_projects.empty()) ok &= filter::has_project(cfg_.filter_projects, task);
        if (ok && !cfg_.filter_contexts.empty()) ok &= filter::has_context(cfg_.filter_contexts, task);

        if (ok && cfg_.filter_due_date.has_value()) 
            ok &= filter::before_date(cfg_.filter_due_date.value(), task);

        return ok;
    }

    void Manager::print_task(const Task& task) {
        std::string priority = task.priority == ' ' ? "" : " (" + std::string(1, task.priority) + ")";
        //fmt::print("{}{} {}\n", task.number, priority, task.text);
        std::cout << fmt::format("{}{} {}", task.number, priority, task.text) << std::endl; // for capturing in test
    }
}
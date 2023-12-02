#pragma once

#include <memory>
#include <mutex>

#include "task/task_export.h"
#include "task/task.h"
#include "task/config.h"
#include "task/parser.h"

namespace task {

    class TASK_EXPORT Manager {
    public:
        static Manager& instance() {
            static std::unique_ptr<Manager> instance;
            static std::once_flag once;
            std::call_once(once, [] { instance.reset(new Manager); });
            return *instance.get();
        };
         
        Manager(Manager const&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager const&) = delete;
        Manager& operator=(Manager&&) = delete;
        ~Manager();

        Config& config();

        bool add_task(const std::string& description);
        bool done_task(const std::size_t number);
        bool delete_task(const std::size_t number);
        bool archive_task();

        bool list_task();

    private:
        Manager();

        std::string temp_filename();
        std::string backup_filename();

        std::string default_archive_filename();

        std::optional<std::pair<std::ifstream, std::ofstream>> begin_editing_file();
        bool finish_editing_file(std::ifstream&, std::ofstream&);

        void fill_task(Task& task);
        bool check_filter(const Task& task);
        void print_task(const Task& task);

    private:
        Config cfg_;
        Parser parser_;
    };
}
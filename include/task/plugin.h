#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "task/task_export.h"
#include "task/task.h"

namespace task {

    class TASK_EXPORT Plugin {
    public:
        enum class Type {
            kAfterDone = 0,
        };

        static Plugin& instance() {
            static std::unique_ptr<Plugin> instance;
            static std::once_flag once;
            std::call_once(once, [] { instance.reset(new Plugin); });
            return *instance.get();
        }

        Plugin(Plugin const&) = delete;
        Plugin(Plugin&&) = delete;
        Plugin& operator=(Plugin const&) = delete;
        Plugin& operator=(Plugin&&) = delete;
        ~Plugin();

        void set_plugin_path(std::filesystem::path path);

        void execute(Type type, const Task& task);

    private:
        Plugin();

        void scan();
        void load(const std::filesystem::path& path);

    private:
        std::filesystem::path plugin_path_;

        struct Entry {
            std::string name;
            void* handle;
            std::function<void(const char*)> after_done;
        };
        std::unordered_map<std::string, Entry> entries_;
    };
}
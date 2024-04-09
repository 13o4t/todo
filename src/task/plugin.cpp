#include "task/plugin.h"

#include <algorithm>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include "task/manager.h"
#include "task/parser.h"

namespace task {

    Plugin::Plugin() : plugin_path_(std::filesystem::current_path() / "plugins") {}

    Plugin::~Plugin() {
        for (auto& entry : entries_) {
#ifdef _WIN32
            ::FreeLibrary((HMODULE)entry.second.handle);
#else
            dlclose(entry.second.handle);
#endif
        }
    }

    void Plugin::set_plugin_path(std::filesystem::path path) {
        plugin_path_ = path;
    }

    void Plugin::scan() {
        if (!std::filesystem::exists(plugin_path_)) return;

        for (auto& file : std::filesystem::directory_iterator{plugin_path_}) {
            load(file.path());
        }
    }

    void Plugin::load(const std::filesystem::path& path) {
#if defined(__APPLE__)
        if (path.extension() != ".dylib") return;
#elif defined(_WIN32)
        if (path.extension() != ".dll") return;
#else
        if (path.extension() != ".so") return;
#endif

        Entry entry;
        entry.name = path.filename().string();

#if defined(_WIN32)
        entry.handle = (void*)::LoadLibraryW(path.wstring().c_str());
        if (!entry.handle || entry.handle == INVALID_HANDLE_VALUE) return;

        entry.after_done = (void(*)(const char*, const char*))::GetProcAddress((HMODULE)entry.handle, "after_done");
#else
        entry.handle = dlopen(path.string().c_str(), RTLD_LAZY);
        if (!entry.handle) return;

        entry.after_done = (void(*)(const char*, const char*))dlsym(entry.handle, "after_done");
#endif

        entries_.insert(std::make_pair(entry.name, std::move(entry)));
    }

    void Plugin::execute(Type type, const Task& task) {
        if (entries_.empty()) scan();
        if (entries_.empty()) return;

        Parser parser;
        std::string description = parser.to_string(task);
        nlohmann::json config_json = Manager::instance().config();
        std::string config_str = config_json.dump();

        for (auto& entry : entries_) {
            if (type == Type::kAfterDone) {
                if (entry.second.after_done) {
                    spdlog::debug("call after done plugin: {}", entry.first);
                    entry.second.after_done(description.c_str(), config_str.c_str());
                    spdlog::debug("finish call after done plugin: {}", entry.first);
                }
            }
        }
    }
}
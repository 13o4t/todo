#include "task/config.h"

namespace task {

    void to_json(nlohmann::json& j, const Config& config) {
        j = nlohmann::json{
            {"file", config.file},
            {"archive_file", config.archive_file},
            {"filter_projects", config.filter_projects},
            {"filter_contexts", config.filter_contexts},
        };
        if (config.filter_due_date.has_value())
            j["filter_due_date"] = config.filter_due_date.value();
    }

    void from_json(const nlohmann::json& j, Config& config) {
        j.at("file").get_to(config.file);
        j.at("archive_file").get_to(config.archive_file);
        j.at("filter_projects").get_to(config.filter_projects);
        j.at("filter_contexts").get_to(config.filter_contexts);
        if (j.contains("filter_due_date")) {
            config.filter_due_date = j.value("filter_due_date", "");
        } else {
            config.filter_due_date = std::nullopt;
        }
    }
}
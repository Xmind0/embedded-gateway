#include "json_utils.h"

bool parse_json(const std::string& str, nlohmann::json& out_json) {
    try {
        out_json = nlohmann::json::parse(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::string dump_json(const nlohmann::json& json_obj) {
    return json_obj.dump();
}

bool get_json_string(const nlohmann::json& json_obj, const std::string& key, std::string& out_value) {
    if (json_obj.contains(key) && json_obj[key].is_string()) {
        out_value = json_obj[key].get<std::string>();
        return true;
    }
    return false;
} 
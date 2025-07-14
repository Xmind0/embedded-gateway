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

bool get_json_int(const nlohmann::json& json_obj, const std::string& key, int& out_value) {
    if (json_obj.contains(key) && json_obj[key].is_number()) {
        out_value = json_obj[key].get<int>();
        return true;
    }
    return false;
}

bool get_json_bool(const nlohmann::json& json_obj, const std::string& key, bool& out_value) {
    if (json_obj.contains(key) && json_obj[key].is_boolean()) {
        out_value = json_obj[key].get<bool>();
        return true;
    }
    return false;
}

// 消息类型检查
bool is_request(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "request";
}

bool is_task(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "task";
}

bool is_response(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "response";
}

bool is_error(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "error";
}

bool is_heartbeat(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "heartbeat";
}

bool is_status(const nlohmann::json& json_obj) {
    std::string type;
    return get_json_string(json_obj, "type", type) && type == "status";
}

// 消息构建函数
nlohmann::json create_request(const std::string& id, const std::string& model, const std::string& prompt, int max_tokens, bool stream) {
    nlohmann::json json_obj;
    json_obj["type"] = "request";
    json_obj["id"] = id;
    json_obj["model"] = model;
    json_obj["prompt"] = prompt;
    json_obj["max_tokens"] = max_tokens;
    json_obj["stream"] = stream;
    return json_obj;
}

nlohmann::json create_task(const std::string& id, int client_socket, const std::string& model, const std::string& prompt, int max_tokens, bool stream) {
    nlohmann::json json_obj;
    json_obj["type"] = "task";
    json_obj["id"] = id;
    json_obj["client_socket"] = client_socket;
    json_obj["model"] = model;
    json_obj["prompt"] = prompt;
    json_obj["max_tokens"] = max_tokens;
    json_obj["stream"] = stream;
    return json_obj;
}

nlohmann::json create_response(const std::string& id, const std::string& result, bool finished) {
    nlohmann::json json_obj;
    json_obj["type"] = "response";
    json_obj["id"] = id;
    json_obj["result"] = result;
    json_obj["finished"] = finished;
    return json_obj;
}

nlohmann::json create_stream_response(const std::string& id, int client_socket, const std::string& token, bool finished) {
    nlohmann::json json_obj;
    json_obj["type"] = "response";
    json_obj["id"] = id;
    json_obj["client_socket"] = client_socket;
    json_obj["token"] = token;
    json_obj["finished"] = finished;
    return json_obj;
}

nlohmann::json create_error(const std::string& id, const std::string& message) {
    nlohmann::json json_obj;
    json_obj["type"] = "error";
    json_obj["id"] = id;
    json_obj["message"] = message;
    return json_obj;
}

nlohmann::json create_client_error(const std::string& id, const std::string& message) {
    nlohmann::json json_obj;
    json_obj["type"] = "error";
    json_obj["id"] = id;
    json_obj["message"] = message;
    return json_obj;
}

nlohmann::json create_heartbeat() {
    nlohmann::json json_obj;
    json_obj["type"] = "heartbeat";
    return json_obj;
}

nlohmann::json create_status(const std::string& node_id, bool available, float load) {
    nlohmann::json json_obj;
    json_obj["type"] = "status";
    json_obj["node_id"] = node_id;
    json_obj["available"] = available;
    json_obj["load"] = load;
    return json_obj;
} 

// 新增：消息类序列化/反序列化
bool parse_request_message(const std::string& json_str, RequestMessage& out_msg) {
    nlohmann::json json_obj;
    if (!parse_json(json_str, json_obj) || !is_request(json_obj)) return false;
    out_msg.from_json(json_obj);
    return true;
}

bool parse_response_message(const std::string& json_str, ResponseMessage& out_msg) {
    nlohmann::json json_obj;
    if (!parse_json(json_str, json_obj) || !is_response(json_obj)) return false;
    out_msg.from_json(json_obj);
    return true;
}

std::string dump_request_message(const RequestMessage& msg) {
    return dump_json(msg.to_json());
}

std::string dump_response_message(const ResponseMessage& msg) {
    return dump_json(msg.to_json());
} 
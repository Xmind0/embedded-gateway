#pragma once
#include <string>
#include <nlohmann/json.hpp>

// 解析JSON字符串，返回nlohmann::json对象
bool parse_json(const std::string& str, nlohmann::json& out_json);

// 生成JSON字符串
std::string dump_json(const nlohmann::json& json_obj);

// 从JSON对象提取字符串字段
bool get_json_string(const nlohmann::json& json_obj, const std::string& key, std::string& out_value);

// 从JSON对象提取整数字段
bool get_json_int(const nlohmann::json& json_obj, const std::string& key, int& out_value);

// 从JSON对象提取布尔字段
bool get_json_bool(const nlohmann::json& json_obj, const std::string& key, bool& out_value);

// 消息类型检查
bool is_request(const nlohmann::json& json_obj);
bool is_task(const nlohmann::json& json_obj);
bool is_response(const nlohmann::json& json_obj);
bool is_error(const nlohmann::json& json_obj);
bool is_heartbeat(const nlohmann::json& json_obj);
bool is_status(const nlohmann::json& json_obj);

// 消息构建函数
nlohmann::json create_request(const std::string& id, const std::string& model, const std::string& prompt, int max_tokens = 1000, bool stream = true);
nlohmann::json create_task(const std::string& id, int client_socket, const std::string& model, const std::string& prompt, int max_tokens = 1000, bool stream = true);
nlohmann::json create_response(const std::string& id, const std::string& result, bool finished = true);
nlohmann::json create_stream_response(const std::string& id, int client_socket, const std::string& token, bool finished = false);
nlohmann::json create_error(const std::string& id, const std::string& message);
nlohmann::json create_client_error(const std::string& id, const std::string& message);
nlohmann::json create_heartbeat();
nlohmann::json create_status(const std::string& node_id, bool available, float load = 0.0f); 
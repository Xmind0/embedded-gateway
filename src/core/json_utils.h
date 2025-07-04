#pragma once
#include <string>
#include <nlohmann/json.hpp>

// 解析JSON字符串，返回nlohmann::json对象
bool parse_json(const std::string& str, nlohmann::json& out_json);

// 生成JSON字符串
std::string dump_json(const nlohmann::json& json_obj);

// 从JSON对象提取字符串字段
bool get_json_string(const nlohmann::json& json_obj, const std::string& key, std::string& out_value); 
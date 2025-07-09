#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "json_utils.h"

// 消息类型枚举
enum class MessageType {
    REQUEST,
    TASK,
    RESPONSE,
    ERROR,
    HEARTBEAT,
    STATUS,
    UNKNOWN
};

// 消息结构体
struct Message {
    MessageType type;
    std::string id;
    std::string model;
    std::string prompt;
    int max_tokens;
    bool stream;
    int client_socket;
    std::string token;
    std::string result;
    bool finished;
    std::string message;
    std::string node_id;
    bool available;
    float load;
    
    Message() : type(MessageType::UNKNOWN), max_tokens(1000), stream(true), 
                client_socket(-1), finished(false), available(false), load(0.0f) {}
};

class MessageHandler {
public:
    // 解析JSON消息
    static bool parse_message(const std::string& json_str, Message& out_msg);
    
    // 构建JSON消息
    static std::string build_request(const std::string& id, const std::string& model, 
                                   const std::string& prompt, int max_tokens = 1000, bool stream = true);
    
    static std::string build_task(const std::string& id, int client_socket, const std::string& model,
                                 const std::string& prompt, int max_tokens = 1000, bool stream = true);
    
    static std::string build_response(const std::string& id, const std::string& result, bool finished = true);
    
    static std::string build_stream_response(const std::string& id, int client_socket, 
                                           const std::string& token, bool finished = false);
    
    static std::string build_error(const std::string& id, const std::string& message);
    
    static std::string build_client_error(const std::string& id, const std::string& message);
    
    static std::string build_heartbeat();
    
    static std::string build_status(const std::string& node_id, bool available, float load = 0.0f);
    
    // 消息类型检查
    static bool is_request_message(const std::string& json_str);
    static bool is_task_message(const std::string& json_str);
    static bool is_response_message(const std::string& json_str);
    static bool is_error_message(const std::string& json_str);
    static bool is_heartbeat_message(const std::string& json_str);
    static bool is_status_message(const std::string& json_str);
    
private:
    static MessageType get_message_type(const nlohmann::json& json_obj);
}; 
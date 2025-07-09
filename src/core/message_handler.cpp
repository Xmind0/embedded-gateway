#include "message_handler.h"

bool MessageHandler::parse_message(const std::string& json_str, Message& out_msg) {
    nlohmann::json json_obj;
    if (!parse_json(json_str, json_obj)) {
        return false;
    }
    
    out_msg.type = get_message_type(json_obj);
    
    // 解析通用字段
    get_json_string(json_obj, "id", out_msg.id);
    get_json_string(json_obj, "model", out_msg.model);
    get_json_string(json_obj, "prompt", out_msg.prompt);
    get_json_int(json_obj, "max_tokens", out_msg.max_tokens);
    get_json_bool(json_obj, "stream", out_msg.stream);
    get_json_int(json_obj, "client_socket", out_msg.client_socket);
    get_json_string(json_obj, "token", out_msg.token);
    get_json_string(json_obj, "result", out_msg.result);
    get_json_bool(json_obj, "finished", out_msg.finished);
    get_json_string(json_obj, "message", out_msg.message);
    get_json_string(json_obj, "node_id", out_msg.node_id);
    get_json_bool(json_obj, "available", out_msg.available);
    
    // 解析load字段（float类型）
    if (json_obj.contains("load") && json_obj["load"].is_number()) {
        out_msg.load = json_obj["load"].get<float>();
    }
    
    return true;
}

std::string MessageHandler::build_request(const std::string& id, const std::string& model, 
                                        const std::string& prompt, int max_tokens, bool stream) {
    nlohmann::json json_obj = create_request(id, model, prompt, max_tokens, stream);
    return dump_json(json_obj);
}

std::string MessageHandler::build_task(const std::string& id, int client_socket, const std::string& model,
                                      const std::string& prompt, int max_tokens, bool stream) {
    nlohmann::json json_obj = create_task(id, client_socket, model, prompt, max_tokens, stream);
    return dump_json(json_obj);
}

std::string MessageHandler::build_response(const std::string& id, const std::string& result, bool finished) {
    nlohmann::json json_obj = create_response(id, result, finished);
    return dump_json(json_obj);
}

std::string MessageHandler::build_stream_response(const std::string& id, int client_socket, 
                                                const std::string& token, bool finished) {
    nlohmann::json json_obj = create_stream_response(id, client_socket, token, finished);
    return dump_json(json_obj);
}

std::string MessageHandler::build_error(const std::string& id, const std::string& message) {
    nlohmann::json json_obj = create_error(id, message);
    return dump_json(json_obj);
}

std::string MessageHandler::build_client_error(const std::string& id, const std::string& message) {
    nlohmann::json json_obj = create_client_error(id, message);
    return dump_json(json_obj);
}

std::string MessageHandler::build_heartbeat() {
    nlohmann::json json_obj = create_heartbeat();
    return dump_json(json_obj);
}

std::string MessageHandler::build_status(const std::string& node_id, bool available, float load) {
    nlohmann::json json_obj = create_status(node_id, available, load);
    return dump_json(json_obj);
}

bool MessageHandler::is_request_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_request(json_obj);
}

bool MessageHandler::is_task_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_task(json_obj);
}

bool MessageHandler::is_response_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_response(json_obj);
}

bool MessageHandler::is_error_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_error(json_obj);
}

bool MessageHandler::is_heartbeat_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_heartbeat(json_obj);
}

bool MessageHandler::is_status_message(const std::string& json_str) {
    nlohmann::json json_obj;
    return parse_json(json_str, json_obj) && is_status(json_obj);
}

MessageType MessageHandler::get_message_type(const nlohmann::json& json_obj) {
    if (is_request(json_obj)) return MessageType::REQUEST;
    if (is_task(json_obj)) return MessageType::TASK;
    if (is_response(json_obj)) return MessageType::RESPONSE;
    if (is_error(json_obj)) return MessageType::ERROR;
    if (is_heartbeat(json_obj)) return MessageType::HEARTBEAT;
    if (is_status(json_obj)) return MessageType::STATUS;
    return MessageType::UNKNOWN;
} 
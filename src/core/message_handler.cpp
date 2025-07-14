#include "message_handler.h"

// Message类实现
Message::Message()
    : type(MessageType::UNKNOWN), max_tokens(1000), stream(true),
      client_socket(-1), finished(false), available(false), load(0.0f) {}

MessageType Message::getType() const { return type; }
void Message::setType(MessageType t) { type = t; }
const std::string& Message::getId() const { return id; }
void Message::setId(const std::string& v) { id = v; }
const std::string& Message::getModel() const { return model; }
void Message::setModel(const std::string& v) { model = v; }
const std::string& Message::getPrompt() const { return prompt; }
void Message::setPrompt(const std::string& v) { prompt = v; }
int Message::getMaxTokens() const { return max_tokens; }
void Message::setMaxTokens(int v) { max_tokens = v; }
bool Message::getStream() const { return stream; }
void Message::setStream(bool v) { stream = v; }
int Message::getClientSocket() const { return client_socket; }
void Message::setClientSocket(int v) { client_socket = v; }
const std::string& Message::getToken() const { return token; }
void Message::setToken(const std::string& v) { token = v; }
const std::string& Message::getResult() const { return result; }
void Message::setResult(const std::string& v) { result = v; }
bool Message::getFinished() const { return finished; }
void Message::setFinished(bool v) { finished = v; }
const std::string& Message::getMessage() const { return message; }
void Message::setMessage(const std::string& v) { message = v; }
const std::string& Message::getNodeId() const { return node_id; }
void Message::setNodeId(const std::string& v) { node_id = v; }
bool Message::getAvailable() const { return available; }
void Message::setAvailable(bool v) { available = v; }
float Message::getLoad() const { return load; }
void Message::setLoad(float v) { load = v; }

void Message::from_json(const nlohmann::json& json_obj) {
    // 类型判断由外部决定
    get_json_string(json_obj, "id", id);
    get_json_string(json_obj, "model", model);
    get_json_string(json_obj, "prompt", prompt);
    get_json_int(json_obj, "max_tokens", max_tokens);
    get_json_bool(json_obj, "stream", stream);
    get_json_int(json_obj, "client_socket", client_socket);
    get_json_string(json_obj, "token", token);
    get_json_string(json_obj, "result", result);
    get_json_bool(json_obj, "finished", finished);
    get_json_string(json_obj, "message", message);
    get_json_string(json_obj, "node_id", node_id);
    get_json_bool(json_obj, "available", available);
    if (json_obj.contains("load") && json_obj["load"].is_number()) {
        load = json_obj["load"].get<float>();
    }
}

nlohmann::json Message::to_json() const {
    nlohmann::json json_obj;
    // type 需由子类或外部补充
    json_obj["id"] = id;
    json_obj["model"] = model;
    json_obj["prompt"] = prompt;
    json_obj["max_tokens"] = max_tokens;
    json_obj["stream"] = stream;
    json_obj["client_socket"] = client_socket;
    json_obj["token"] = token;
    json_obj["result"] = result;
    json_obj["finished"] = finished;
    json_obj["message"] = message;
    json_obj["node_id"] = node_id;
    json_obj["available"] = available;
    json_obj["load"] = load;
    return json_obj;
}

// RequestMessage实现
RequestMessage::RequestMessage() {
    setType(MessageType::REQUEST);
}
void RequestMessage::from_json(const nlohmann::json& json_obj) {
    Message::from_json(json_obj);
    setType(MessageType::REQUEST);
}
nlohmann::json RequestMessage::to_json() const {
    nlohmann::json json_obj = Message::to_json();
    json_obj["type"] = "request";
    return json_obj;
}

// ResponseMessage实现
ResponseMessage::ResponseMessage() {
    setType(MessageType::RESPONSE);
}
void ResponseMessage::from_json(const nlohmann::json& json_obj) {
    Message::from_json(json_obj);
    setType(MessageType::RESPONSE);
}
nlohmann::json ResponseMessage::to_json() const {
    nlohmann::json json_obj = Message::to_json();
    json_obj["type"] = "response";
    return json_obj;
}

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
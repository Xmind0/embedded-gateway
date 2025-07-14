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
class Message {
public:
    Message();
    virtual ~Message() = default;
    MessageType getType() const;
    void setType(MessageType t);
    const std::string& getId() const;
    void setId(const std::string& id);
    const std::string& getModel() const;
    void setModel(const std::string& model);
    const std::string& getPrompt() const;
    void setPrompt(const std::string& prompt);
    int getMaxTokens() const;
    void setMaxTokens(int max_tokens);
    bool getStream() const;
    void setStream(bool stream);
    int getClientSocket() const;
    void setClientSocket(int sock);
    const std::string& getToken() const;
    void setToken(const std::string& token);
    const std::string& getResult() const;
    void setResult(const std::string& result);
    bool getFinished() const;
    void setFinished(bool finished);
    const std::string& getMessage() const;
    void setMessage(const std::string& msg);
    const std::string& getNodeId() const;
    void setNodeId(const std::string& node_id);
    bool getAvailable() const;
    void setAvailable(bool available);
    float getLoad() const;
    void setLoad(float load);

    // JSON序列化/反序列化
    virtual void from_json(const nlohmann::json& json_obj);
    virtual nlohmann::json to_json() const;

protected:
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
};

// RequestMessage
class RequestMessage : public Message {
public:
    RequestMessage();
    void from_json(const nlohmann::json& json_obj) override;
    nlohmann::json to_json() const override;
};

// ResponseMessage
class ResponseMessage : public Message {
public:
    ResponseMessage();
    void from_json(const nlohmann::json& json_obj) override;
    nlohmann::json to_json() const override;
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
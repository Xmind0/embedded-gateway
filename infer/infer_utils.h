#pragma once
#include <string>
#include <nlohmann/json.hpp>

// 配置选项
#define SERVER_IP "192.168.1.100"
#define SERVER_PORT 9000
#define INFER_ENGINE_SOCK_PATH "/tmp/infer_engine.sock"
#define INFER_MAX_JSON_SIZE 2048

// 请求/响应结构体（与主项目一致）
struct InferRequest {
    std::string id;
    std::string model;
    std::string prompt;
    int max_tokens;
    bool stream;
};

struct InferResponse {
    std::string id;
    std::string result;
    bool finished;
};

// JSON 序列化/反序列化
inline bool parse_infer_request(const std::string& str, InferRequest& req) {
    try {
        auto j = nlohmann::json::parse(str);
        req.id = j.value("id", "");
        req.model = j.value("model", "");
        req.prompt = j.value("prompt", "");
        req.max_tokens = j.value("max_tokens", 0);
        req.stream = j.value("stream", false);
        return true;
    } catch (...) { return false; }
}

inline std::string dump_infer_response(const InferResponse& resp) {
    nlohmann::json j;
    j["type"] = "response";
    j["id"] = resp.id;
    j["result"] = resp.result;
    j["finished"] = resp.finished;
    return j.dump();
} 
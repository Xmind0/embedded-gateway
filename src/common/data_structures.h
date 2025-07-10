#pragma once

#include <cstring>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <cstdlib>
#include <random>
#include <sstream>
#include <etl/string.h>

// 简单的队列实现
template<typename T, int MAX_SIZE>
class SimpleQueue {
public:
    SimpleQueue() : head(0), tail(0), size(0) {}
    
    bool push(const T& item) {
        if (size >= MAX_SIZE) return false;
        data[tail] = item;
        // 环形队列：当tail到达数组末尾时，通过模运算回到数组开头
        // 这样可以重复利用数组空间，避免频繁的内存分配和释放
        tail = (tail + 1) % MAX_SIZE;
        size++;
        return true;
    }
    
    bool pop(T& item) {
        if (size == 0) return false;
        item = data[head];
        // 环形队列：当head到达数组末尾时，通过模运算回到数组开头
        head = (head + 1) % MAX_SIZE;
        size--;
        return true;
    }
    
    bool empty() const { return size == 0; }
    int getSize() const { return size; }
    
    // 添加安全检查
    bool isFull() const { return size >= MAX_SIZE; }
    int getMaxSize() const { return MAX_SIZE; }
    
private:
    T data[MAX_SIZE];
    int head, tail, size;
};

// 消息基类
class BaseMessage {
public:
    // 这里使用 explicit 是为了防止 BaseMessage(int sock_id) 构造函数发生隐式类型转换。
    // 比如：BaseMessage msg = 5; // 如果没有 explicit，会被允许，可能导致意外错误。
    explicit BaseMessage(int sock_id) : socket_id(sock_id) {}
    virtual ~BaseMessage() = default;
    
    // 禁用拷贝构造和赋值操作符
    BaseMessage(const BaseMessage&) = delete;
    BaseMessage& operator=(const BaseMessage&) = delete;
    
    int getSocketId() const { return socket_id; }
private:
    int socket_id;
};

// 任务状态枚举
enum class TaskStatus {
    PENDING,        // 等待处理
    ASSIGNING,      // 正在分配
    PROCESSING,     // 推理中
    COMPLETED,      // 完成
    FAILED,         // 失败
    TIMEOUT,        // 超时
    CANCELLED       // 取消
};

// 推理请求消息
class InferenceRequest : public BaseMessage {
public:
    InferenceRequest(int sock_id, const char* request_id, const char* data, int priority = 0)
        : BaseMessage(sock_id), priority(priority), request_id(nullptr), data(nullptr) {
        // 添加空指针检查
        if (request_id) {
            this->request_id = strdup(request_id);
        }
        if (data) {
            this->data = strdup(data);
        }
        status = TaskStatus::PENDING;
        create_time = getCurrentTimestamp();
        assign_time = 0;
        complete_time = 0;
    }
    
    ~InferenceRequest() override {
        if (request_id) {
            free(request_id);
            request_id = nullptr;
        }
        if (data) {
            free(data);
            data = nullptr;
        }
    }
    
    // 禁用拷贝构造和赋值操作符，防止内存泄漏
    InferenceRequest(const InferenceRequest&) = delete;
    InferenceRequest& operator=(const InferenceRequest&) = delete;
    
    const char* getRequestId() const { return request_id; }
    const char* getData() const { return data; }
    int getPriority() const { return priority; }
    TaskStatus getStatus() const { return status; }
    void setStatus(TaskStatus s) { status = s; }
    
    long long getCreateTime() const { return create_time; }
    long long getAssignTime() const { return assign_time; }
    long long getCompleteTime() const { return complete_time; }
    
    void setAssignTime(long long time) { assign_time = time; }
    void setCompleteTime(long long time) { complete_time = time; }
    
private:
    char* request_id;
    char* data;
    int priority;
    TaskStatus status;
    long long create_time;
    long long assign_time;
    long long complete_time;
    
    long long getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

// 推理响应消息
class InferenceResponse : public BaseMessage {
public:
    InferenceResponse(int sock_id, const char* request_id, const char* result, bool success, const char* error_msg = nullptr)
        : BaseMessage(sock_id), success(success), request_id(nullptr), result(nullptr), error_msg(nullptr) {
        // 添加空指针检查
        if (request_id) {
            this->request_id = strdup(request_id);
        }
        if (result) {
            this->result = strdup(result);
        }
        if (error_msg) {
            this->error_msg = strdup(error_msg);
        }
        complete_time = getCurrentTimestamp();
    }
    
    ~InferenceResponse() override {
        if (request_id) {
            free(request_id);
            request_id = nullptr;
        }
        if (result) {
            free(result);
            result = nullptr;
        }
        if (error_msg) {
            free(error_msg);
            error_msg = nullptr;
        }
    }
    
    // 禁用拷贝构造和赋值操作符，防止内存泄漏
    InferenceResponse(const InferenceResponse&) = delete;
    InferenceResponse& operator=(const InferenceResponse&) = delete;
    
    const char* getRequestId() const { return request_id; }
    const char* getResult() const { return result; }
    bool isSuccess() const { return success; }
    const char* getErrorMessage() const { return error_msg; }
    long long getCompleteTime() const { return complete_time; }
    
private:
    char* request_id;
    char* result;
    bool success;
    char* error_msg;
    long long complete_time;
    
    long long getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

// 客户端请求消息
class RequestMessage : public BaseMessage {
public:
    RequestMessage(int sock_id, const char* ip, const char* data)
        : BaseMessage(sock_id), client_ip(nullptr), request_data(nullptr) {
        // 添加空指针检查
        if (ip) {
            client_ip = strdup(ip);
        }
        if (data) {
            request_data = strdup(data);
        }
    }
    ~RequestMessage() override {
        if (client_ip) {
            free(client_ip);
            client_ip = nullptr;
        }
        if (request_data) {
            free(request_data);
            request_data = nullptr;
        }
    }
    
    // 禁用拷贝构造和赋值操作符，防止内存泄漏
    RequestMessage(const RequestMessage&) = delete;
    RequestMessage& operator=(const RequestMessage&) = delete;
    
    const char* getClientIp() const { return client_ip; }
    const char* getRequestData() const { return request_data; }
private:
    char* client_ip;
    char* request_data;
};

// 服务端响应消息 - 使用自定义队列
class ResponseMessage : public BaseMessage {
public:
    static const int MAX_RESPONSES = 100;
    
    explicit ResponseMessage(int sock_id) : BaseMessage(sock_id) {}
    
    void addResponseData(const char* data) {
        if (data) {
            response_queue.push(strdup(data));
        }
    }
    
    bool getNextResponse(char*& data) {
        return response_queue.pop(data);
    }
    
    bool hasResponses() const {
        return !response_queue.empty();
    }
    
    int getResponseCount() const {
        return response_queue.getSize();
    }
    
    ~ResponseMessage() override {
        char* data;
        while (response_queue.pop(data)) {
            if (data) {
                free(data);
                data = nullptr;
            }
        }
    }
    
    // 禁用拷贝构造和赋值操作符，防止内存泄漏
    ResponseMessage(const ResponseMessage&) = delete;
    ResponseMessage& operator=(const ResponseMessage&) = delete;
    
private:
    SimpleQueue<char*, MAX_RESPONSES> response_queue;
};

// 线程安全响应追踪器 - 使用std::unordered_map
class ResponseTracker {
public:
    void setPending(int client_socket) {
        std::lock_guard<std::mutex> lock(response_mutex);
        response_map[client_socket] = ResponseMessage(client_socket);
    }
    
    void setResponse(int client_socket, const char* data) {
        std::lock_guard<std::mutex> lock(response_mutex);
        auto it = response_map.find(client_socket);
        if (it != response_map.end()) {
            it->second.addResponseData(data);
        }
    }
    
    bool getResponse(int client_socket, ResponseMessage& out) {
        std::lock_guard<std::mutex> lock(response_mutex);
        auto it = response_map.find(client_socket);
        if (it != response_map.end()) {
            out = it->second;
            return true;
        }
        return false;
    }
    
    void remove(int client_socket) {
        std::lock_guard<std::mutex> lock(response_mutex);
        response_map.erase(client_socket);
    }
private:
    std::unordered_map<int, ResponseMessage> response_map;
    mutable std::mutex response_mutex;
};

// 客户端消息缓存类（全局队列实现）
class ClientMessageCache {
public:
    static const int MAX_MSGS = 100;

    ~ClientMessageCache() {
        clearMessages();
    }

    void addMessage(RequestMessage* msg) {
        if (msg) {
            queue.push(msg);
        }
    }

    bool getNextMessage(RequestMessage*& out_msg) {
        RequestMessage* tmp = nullptr;
        if (queue.pop(tmp)) {
            if (tmp) {
                out_msg = new RequestMessage(*tmp); // 拷贝构造
                delete tmp; // 销毁队列中的原指针
            } else {
                out_msg = nullptr;
            }
            return true;
        }
        out_msg = nullptr;
        return false;
    }

    void clearMessages() {
        RequestMessage* msg;
        while (queue.pop(msg)) {
            if (msg) {
                delete msg;
                msg = nullptr;
            }
        }
    }

private:
    SimpleQueue<RequestMessage*, MAX_MSGS> queue;
};

// 简单的任务结构体（用于兼容旧接口）
struct Task {
    int client_socket;
    std::string request_data;
    std::string response_data;
    bool success;
    std::string error_msg;
    
    Task(int socket, const std::string& req, const std::string& resp, bool succ, const std::string& err)
        : client_socket(socket), request_data(req), response_data(resp), success(succ), error_msg(err) {}
};

// 生成唯一 request_id（返回 etl::string<64>）
inline etl::string<64> generate_request_id() {
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    long long ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t rnd = dist(rng);
    char buf[64];
    snprintf(buf, 64, "%lld-%llu", ts, (unsigned long long)rnd);
    return etl::string<64>(buf);
} 
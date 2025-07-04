#pragma once
#include <etl/queue.h>
#include <atomic>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include "task.h"
#include <etl/map.h>
#include <etl/string.h>

class InferenceNodeManager; // 前向声明

// 单向链表节点
struct TokenNode {
    char* token;
    TokenNode* next;
    TokenNode(const char* t) : token(nullptr), next(nullptr) {
        if (t) {
            size_t len = strlen(t) + 1;
            token = (char*)malloc(len);
            if (token) {
                strcpy(token, t);
            }
        }
    }
    ~TokenNode() {
        if (token) {
            free(token);
            token = nullptr;
        }
    }
};

// 自定义链表类
class TokenList {
public:
    TokenList() : head(nullptr), tail(nullptr), size(0), output_ptr(nullptr) {}
    ~TokenList() {
        clear();
    }
    
    // 添加token到链表尾部
    void addToken(const char* token) {
        if (!token) return;
        TokenNode* node = new TokenNode(token);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
    }
    
    // 获取链表头
    TokenNode* getHead() const { return head; }
    
    // 获取链表大小
    int getSize() const { return size; }
    
    // 清空链表
    void clear() {
        TokenNode* cur = head;
        while (cur) {
            TokenNode* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        size = 0;
        output_ptr = nullptr;
    }
    
    // 输出相关函数
    // 重置输出指针到链表头
    void resetOutput() {
        output_ptr = head;
    }
    
    // 获取下一个token，指针自动移动
    const char* getNextToken() {
        if (!output_ptr) return nullptr;
        const char* token = output_ptr->token;
        output_ptr = output_ptr->next;
        return token;
    }
    
    // 检查是否还有更多token
    bool hasMoreTokens() const {
        return output_ptr != nullptr;
    }
    
    // 获取当前输出位置
    TokenNode* getCurrentOutputPos() const {
        return output_ptr;
    }
    
private:
    TokenNode* head;
    TokenNode* tail;
    int size;
    TokenNode* output_ptr;  // 输出遍历指针
};

class TaskManager {
public:
    static constexpr size_t MAX_TASKS = 128;
    TaskManager();
    ~TaskManager();

    // 启动任务管理
    bool start(InferenceNodeManager* node_manager = nullptr);
    void stop();
    bool isRunning() const { return running.load(); }

    // 客户端推送请求
    void pushRequest(int client_socket, const std::string& data);
    // 节点推送响应
    void pushResponse(int client_socket, const std::string& data, bool success, const std::string& error_msg = "");
    // 取出已完成响应
    bool popFinishedResponse(Task& task);

    // token流式接收接口
    void addToken(const etl::string<64>& request_id, const char* token);
    // 获取链表
    TokenList* getTokenList(const etl::string<64>& request_id);
    // 清理链表
    void clearTokenList(const etl::string<64>& request_id);

private:
    void taskLoop();
    void responseLoop();

    std::atomic<bool> running;
    std::thread task_thread;
    std::thread response_thread;

    etl::queue<Task, MAX_TASKS> input_queue;
    etl::queue<Task, MAX_TASKS> output_queue;

    InferenceNodeManager* node_manager_;

    // request_id -> token链表
    etl::map<etl::string<64>, TokenList*, MAX_TASKS> token_map_;
}; 
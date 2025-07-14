#pragma once
#include <queue>
#include <atomic>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include <map>
#include <string>
#include "task_cache.h"
#include "task_queue.h"
#include "data_structures.h"

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
    TokenList() : head(nullptr), tail(nullptr), size(0), output_ptr(nullptr), is_finished(false) {}
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
    
    // 标记token流结束
    void markFinished() {
        is_finished = true;
    }
    
    // 检查token流是否结束
    bool isFinished() const {
        return is_finished;
    }
    
    // 检查是否还有更多token（包括未结束的流）
    bool hasMoreTokens() const {
        return output_ptr != nullptr || (!is_finished && size > 0);
    }
    
    // 检查是否完全结束（已发送完所有token且流已结束）
    bool isCompletelyFinished() const {
        return is_finished && output_ptr == nullptr;
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
        is_finished = false;
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
    
    // 获取当前输出位置
    TokenNode* getCurrentOutputPos() const {
        return output_ptr;
    }
    
private:
    TokenNode* head;
    TokenNode* tail;
    int size;
    TokenNode* output_ptr;  // 输出遍历指针
    bool is_finished;       // 标记token流是否结束
};

// 主任务管理器 - 协调缓存池和队列
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
    void pushRequest(int client_socket, const RequestMessage& request);
    // 节点推送响应
    void pushResponse(int client_socket, const ResponseMessage& response, bool success, const std::string& error_msg = "");
    // 取出已完成响应
    bool popFinishedResponse(Task& task);

    // token流式接收接口
    void addToken(const std::string& request_id, const char* token);
    // 标记token流结束
    void markTokenStreamFinished(const std::string& request_id);
    // 获取链表
    TokenList* getTokenList(const std::string& request_id);
    // 清理链表
    void clearTokenList(const std::string& request_id);

    // 新的任务管理接口
    TaskContext* createTask(const std::string& request_id, int client_socket, 
                           const RequestMessage& request, int priority = 0);
    TaskContext* getNextPendingTask();
    TaskContext* getTask(const std::string& request_id);
    void completeTask(const std::string& request_id, const std::string& result);
    void failTask(const std::string& request_id, const std::string& error);
    
    // 统计信息
    size_t getPendingTaskCount() const;
    size_t getProcessingTaskCount() const;
    size_t getTotalTaskCount() const;

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
    std::map<std::string, TokenList*> token_map_;
    
    // 分离的缓存池和队列
    TaskCache task_cache_;
    TaskQueue task_queue_;
}; 
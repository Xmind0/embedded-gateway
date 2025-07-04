#pragma once
#include <etl/queue.h>
#include <atomic>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include "task.h"

class InferenceNodeManager; // 前向声明

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

private:
    void taskLoop();
    void responseLoop();

    std::atomic<bool> running;
    std::thread task_thread;
    std::thread response_thread;

    etl::queue<Task, MAX_TASKS> input_queue;
    etl::queue<Task, MAX_TASKS> output_queue;

    InferenceNodeManager* node_manager_;
}; 
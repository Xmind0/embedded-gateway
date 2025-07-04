#include "task_manager.h"
#include "inference_node_manager.h"
#include <etl/string.h>

TaskManager::TaskManager() : running(false), node_manager_(nullptr) {}
TaskManager::~TaskManager() { stop(); }

bool TaskManager::start(InferenceNodeManager* node_manager) {
    if (running.load()) return false;
    node_manager_ = node_manager;
    running.store(true);
    task_thread = std::thread(&TaskManager::taskLoop, this);
    response_thread = std::thread(&TaskManager::responseLoop, this);
    return true;
}

void TaskManager::stop() {
    if (!running.load()) return;
    running.store(false);
    if (task_thread.joinable()) task_thread.join();
    if (response_thread.joinable()) response_thread.join();
}

void TaskManager::pushRequest(int client_socket, const std::string& data) {
    if (!input_queue.full()) {
        input_queue.push(Task{client_socket, data, "", false, ""});
    }
}

void TaskManager::pushResponse(int client_socket, const std::string& data, bool success, const std::string& error_msg) {
    if (!output_queue.full()) {
        output_queue.push(Task{client_socket, "", data, true, error_msg});
    }
}

bool TaskManager::popFinishedResponse(Task& task) {
    if (output_queue.empty()) return false;
    task = output_queue.front();
    output_queue.pop();
    return true;
}

void TaskManager::taskLoop() {
    while (running.load()) {
        if (!input_queue.empty()) {
            Task task = input_queue.front();
            input_queue.pop();
            if (node_manager_) {
                node_manager_->sendToNode(task.client_socket, task.request_data);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void TaskManager::responseLoop() {
    while (running.load()) {
        if (!output_queue.empty()) {
            Task task = output_queue.front();
            output_queue.pop();
            // 这里可以通过 ClientManager 发送响应
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

// 添加token到链表
void TaskManager::addToken(const etl::string<64>& request_id, const char* token) {
    auto it = token_map_.find(request_id);
    if (it == token_map_.end()) {
        TokenList* list = new TokenList();
        list->addToken(token);
        token_map_.insert(std::make_pair(request_id, list));
    } else {
        it->second->addToken(token);
    }
}

// 获取链表
TokenList* TaskManager::getTokenList(const etl::string<64>& request_id) {
    auto it = token_map_.find(request_id);
    if (it != token_map_.end()) return it->second;
    return nullptr;
}

// 清理链表
void TaskManager::clearTokenList(const etl::string<64>& request_id) {
    auto it = token_map_.find(request_id);
    if (it != token_map_.end()) {
        delete it->second;
        token_map_.erase(it);
    }
} 
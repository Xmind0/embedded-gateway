#include "task_manager.h"
#include "inference_node_manager.h"
#include <chrono>

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

void TaskManager::pushRequest(int client_socket, const RequestMessage& request) {
    if (!input_queue.full()) {
        input_queue.push(Task{client_socket, request, ResponseMessage(), false, ""});
    }
}

void TaskManager::pushResponse(int client_socket, const ResponseMessage& response, bool success, const std::string& error_msg) {
    if (!output_queue.full()) {
        output_queue.push(Task{client_socket, RequestMessage(), response, true, error_msg});
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
void TaskManager::addToken(const std::string& request_id, const char* token) {
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
TokenList* TaskManager::getTokenList(const std::string& request_id) {
    auto it = token_map_.find(request_id);
    if (it != token_map_.end()) return it->second;
    return nullptr;
}

// 清理链表
void TaskManager::clearTokenList(const std::string& request_id) {
    auto it = token_map_.find(request_id);
    if (it != token_map_.end()) {
        delete it->second;
        token_map_.erase(it);
    }
}

// 标记token流结束
void TaskManager::markTokenStreamFinished(const std::string& request_id) {
    auto it = token_map_.find(request_id);
    if (it != token_map_.end()) {
        it->second->markFinished();
    }
}

// 新的任务管理接口实现
TaskContext* TaskManager::createTask(const std::string& request_id, int client_socket, const RequestMessage& request, int priority) {
    TaskContext* task = task_cache_.createTask(request_id, client_socket, request, priority);
    if (task) {
        task_queue_.addToPendingQueue(task);
    }
    return task;
}

TaskContext* TaskManager::getNextPendingTask() {
    TaskContext* task = task_queue_.getNextPendingTask();
    if (task) {
        task->status = TaskStatus::PROCESSING;
        task->assign_time = getCurrentTimestamp();
        task_queue_.addToProcessingQueue(task);
    }
    return task;
}

TaskContext* TaskManager::getTask(const std::string& request_id) {
    return task_cache_.getTask(request_id);
}

void TaskManager::completeTask(const std::string& request_id, const std::string& result) {
    TaskContext* task = task_cache_.getTask(request_id);
    if (task) {
        task->response_data = result;
        task->status = TaskStatus::COMPLETED;
        task->complete_time = getCurrentTimestamp();
        
        task_queue_.removeFromProcessingQueue(task);
        task_cache_.completeTask(request_id);
    }
}

void TaskManager::failTask(const std::string& request_id, const std::string& error) {
    TaskContext* task = task_cache_.getTask(request_id);
    if (task) {
        task->error_msg = error;
        task->status = TaskStatus::FAILED;
        task->complete_time = getCurrentTimestamp();
        
        task_queue_.removeFromProcessingQueue(task);
        task_cache_.completeTask(request_id);
    }
}

// 统计信息
size_t TaskManager::getPendingTaskCount() const {
    return task_queue_.getPendingQueueSize();
}

size_t TaskManager::getProcessingTaskCount() const {
    return task_queue_.getProcessingQueueSize();
}

size_t TaskManager::getTotalTaskCount() const {
    return task_cache_.getSize();
}

// 辅助函数
long long TaskManager::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
} 
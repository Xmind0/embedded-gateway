#include "task_cache.h"
#include <chrono>

TaskContext* TaskCache::createTask(const std::string& request_id, int client_socket, 
                                   const std::string& request_data, int priority) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    
    if (task_cache_.size() >= MAX_TASKS) return nullptr;
    
    TaskContext* task = new TaskContext();
    
    // 初始化任务
    task->request_id = request_id;
    task->client_socket = client_socket;
    task->request_data = request_data;
    task->status = TaskStatus::PENDING;
    task->create_time = getCurrentTimestamp();
    task->priority = priority;
    
    // 添加到缓存池
    task_cache_.insert(std::make_pair(request_id, task));
    
    return task;
}

TaskContext* TaskCache::getTask(const std::string& request_id) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = task_cache_.find(request_id);
    return (it != task_cache_.end()) ? it->second : nullptr;
}

void TaskCache::updateTaskStatus(const std::string& request_id, TaskStatus status) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = task_cache_.find(request_id);
    if (it != task_cache_.end()) {
        it->second->status = status;
    }
}

void TaskCache::completeTask(const std::string& request_id) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = task_cache_.find(request_id);
    if (it != task_cache_.end()) {
        TaskContext* task = it->second;
        task_cache_.erase(it);
        delete task;
    }
}

void TaskCache::getTasksByStatus(TaskStatus status, std::vector<TaskContext*>& tasks) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    tasks.clear();
    
    for (auto& pair : task_cache_) {
        if (pair.second->status == status) {
            tasks.push_back(pair.second);
        }
    }
}

size_t TaskCache::getSize() const {
    return task_cache_.size();
}

bool TaskCache::isFull() const {
    return task_cache_.size() >= MAX_TASKS;
}

long long TaskCache::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
} 
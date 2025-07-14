#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <vector>
#include "task_context.h"

// 任务缓存池 - 负责所有任务的存储和查找
class TaskCache {
private:
    static constexpr size_t MAX_TASKS = 128;
    
    // 主缓存池：存储所有任务
    std::unordered_map<std::string, TaskContext*> task_cache_;
    
    std::mutex cache_mutex_;
    
public:
    TaskCache() = default;
    ~TaskCache() = default;
    
    // 创建新任务
    TaskContext* createTask(const std::string& request_id, int client_socket, 
                           const RequestMessage& request, int priority = 0);
    
    // 获取任务
    TaskContext* getTask(const std::string& request_id);
    
    // 更新任务状态
    void updateTaskStatus(const std::string& request_id, TaskStatus status);
    
    // 完成并释放任务
    void completeTask(const std::string& request_id);
    
    // 获取所有指定状态的任务
    void getTasksByStatus(TaskStatus status, std::vector<TaskContext*>& tasks);
    
    // 获取缓存池大小
    size_t getSize() const;
    
    // 检查是否已满
    bool isFull() const;
    
private:
    long long getCurrentTimestamp();
}; 
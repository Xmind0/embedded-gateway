#pragma once
#include <queue>
#include <string>
#include <mutex>
#include "task_context.h"

// 任务队列 - 负责特定状态任务的队列管理
class TaskQueue {
private:
    static constexpr size_t MAX_PENDING_QUEUE = 64;
    static constexpr size_t MAX_PROCESSING_QUEUE = 32;
    
    // 不同状态的队列
    std::queue<TaskContext*> pending_queue_;
    std::queue<TaskContext*> processing_queue_;
    
    std::mutex pending_mutex_;
    std::mutex processing_mutex_;
    
public:
    TaskQueue() = default;
    ~TaskQueue() = default;
    
    // 添加任务到队列
    bool addToPendingQueue(TaskContext* task);
    bool addToProcessingQueue(TaskContext* task);
    
    // 从队列获取任务
    TaskContext* getNextPendingTask();
    TaskContext* getNextProcessingTask();
    
    // 从队列移除任务
    void removeFromPendingQueue(TaskContext* task);
    void removeFromProcessingQueue(TaskContext* task);
    
    // 获取队列状态
    size_t getPendingQueueSize() const;
    size_t getProcessingQueueSize() const;
    bool isPendingQueueEmpty() const;
    bool isProcessingQueueEmpty() const;
    
private:
    // 从队列中移除指定任务
    template<typename QueueType>
    void removeFromQueue(QueueType& queue, TaskContext* target_task);
}; 
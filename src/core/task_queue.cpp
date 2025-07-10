#include "task_queue.h"

bool TaskQueue::addToPendingQueue(TaskContext* task) {
    std::lock_guard<std::mutex> lock(pending_mutex_);
    if (pending_queue_.size() < MAX_PENDING_QUEUE) {
        pending_queue_.push(task);
        return true;
    }
    return false;
}

bool TaskQueue::addToProcessingQueue(TaskContext* task) {
    std::lock_guard<std::mutex> lock(processing_mutex_);
    if (processing_queue_.size() < MAX_PROCESSING_QUEUE) {
        processing_queue_.push(task);
        return true;
    }
    return false;
}

TaskContext* TaskQueue::getNextPendingTask() {
    std::lock_guard<std::mutex> lock(pending_mutex_);
    if (pending_queue_.empty()) return nullptr;
    
    TaskContext* task = pending_queue_.front();
    pending_queue_.pop();
    return task;
}

TaskContext* TaskQueue::getNextProcessingTask() {
    std::lock_guard<std::mutex> lock(processing_mutex_);
    if (processing_queue_.empty()) return nullptr;
    
    TaskContext* task = processing_queue_.front();
    processing_queue_.pop();
    return task;
}

void TaskQueue::removeFromPendingQueue(TaskContext* task) {
    std::lock_guard<std::mutex> lock(pending_mutex_);
    removeFromQueue(pending_queue_, task);
}

void TaskQueue::removeFromProcessingQueue(TaskContext* task) {
    std::lock_guard<std::mutex> lock(processing_mutex_);
    removeFromQueue(processing_queue_, task);
}

size_t TaskQueue::getPendingQueueSize() const {
    return pending_queue_.size();
}

size_t TaskQueue::getProcessingQueueSize() const {
    return processing_queue_.size();
}

bool TaskQueue::isPendingQueueEmpty() const {
    return pending_queue_.empty();
}

bool TaskQueue::isProcessingQueueEmpty() const {
    return processing_queue_.empty();
}

template<typename QueueType>
void TaskQueue::removeFromQueue(QueueType& queue, TaskContext* target_task) {
    QueueType temp_queue;
    
    while (!queue.empty()) {
        TaskContext* current = queue.front();
        queue.pop();
        
        if (current != target_task) {
            temp_queue.push(current);
        }
    }
    
    queue = temp_queue;
} 
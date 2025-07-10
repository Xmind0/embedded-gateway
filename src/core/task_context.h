#pragma once
#include <string>
#include <chrono>
#include "data_structures.h"

// 任务上下文结构
struct TaskContext {
    std::string request_id;
    int client_socket;
    std::string request_data;
    std::string response_data;
    TaskStatus status;
    long long create_time;
    long long assign_time;
    long long complete_time;
    int priority;
    std::string error_msg;
    
    TaskContext() : client_socket(-1), status(TaskStatus::PENDING), 
                   create_time(0), assign_time(0), complete_time(0), priority(0) {}
}; 
#pragma once
#include <string>
#include <chrono>
#include "data_structures.h"
#include "message_handler.h"
#include <nlohmann/json.hpp>

// 任务上下文结构
class TaskContext {
public:
    TaskContext();
    TaskContext(const std::string& request_id, int client_socket, const RequestMessage& request, int priority = 0);
    // getter/setter
    const std::string& getRequestId() const;
    void setRequestId(const std::string& id);
    int getClientSocket() const;
    void setClientSocket(int sock);
    const RequestMessage& getRequest() const;
    void setRequest(const RequestMessage& req);
    const ResponseMessage& getResponse() const;
    void setResponse(const ResponseMessage& resp);
    TaskStatus getStatus() const;
    void setStatus(TaskStatus s);
    long long getCreateTime() const;
    void setCreateTime(long long t);
    long long getAssignTime() const;
    void setAssignTime(long long t);
    long long getCompleteTime() const;
    void setCompleteTime(long long t);
    int getPriority() const;
    void setPriority(int p);
    const std::string& getErrorMsg() const;
    void setErrorMsg(const std::string& msg);
    nlohmann::json to_json() const;
    void from_json(const nlohmann::json& j);
private:
    std::string request_id;
    int client_socket;
    RequestMessage request;
    ResponseMessage response;
    TaskStatus status;
    long long create_time;
    long long assign_time;
    long long complete_time;
    int priority;
    std::string error_msg;
}; 
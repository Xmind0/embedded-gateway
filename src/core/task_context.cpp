#include "task_context.h"

TaskContext::TaskContext()
    : client_socket(-1), status(TaskStatus::PENDING),
      create_time(0), assign_time(0), complete_time(0), priority(0) {}

TaskContext::TaskContext(const std::string& request_id, int client_socket, const RequestMessage& request, int priority)
    : request_id(request_id), client_socket(client_socket), request(request), status(TaskStatus::PENDING),
      create_time(0), assign_time(0), complete_time(0), priority(priority) {}

const std::string& TaskContext::getRequestId() const { return request_id; }
void TaskContext::setRequestId(const std::string& id) { request_id = id; }
int TaskContext::getClientSocket() const { return client_socket; }
void TaskContext::setClientSocket(int sock) { client_socket = sock; }
const RequestMessage& TaskContext::getRequest() const { return request; }
void TaskContext::setRequest(const RequestMessage& req) { request = req; }
const ResponseMessage& TaskContext::getResponse() const { return response; }
void TaskContext::setResponse(const ResponseMessage& resp) { response = resp; }
TaskStatus TaskContext::getStatus() const { return status; }
void TaskContext::setStatus(TaskStatus s) { status = s; }
long long TaskContext::getCreateTime() const { return create_time; }
void TaskContext::setCreateTime(long long t) { create_time = t; }
long long TaskContext::getAssignTime() const { return assign_time; }
void TaskContext::setAssignTime(long long t) { assign_time = t; }
long long TaskContext::getCompleteTime() const { return complete_time; }
void TaskContext::setCompleteTime(long long t) { complete_time = t; }
int TaskContext::getPriority() const { return priority; }
void TaskContext::setPriority(int p) { priority = p; }
const std::string& TaskContext::getErrorMsg() const { return error_msg; }
void TaskContext::setErrorMsg(const std::string& msg) { error_msg = msg; }

nlohmann::json TaskContext::to_json() const {
    nlohmann::json j;
    j["request_id"] = request_id;
    j["client_socket"] = client_socket;
    j["request"] = request.to_json();
    j["response"] = response.to_json();
    j["status"] = static_cast<int>(status);
    j["create_time"] = create_time;
    j["assign_time"] = assign_time;
    j["complete_time"] = complete_time;
    j["priority"] = priority;
    j["error_msg"] = error_msg;
    return j;
}

void TaskContext::from_json(const nlohmann::json& j) {
    request_id = j.value("request_id", "");
    client_socket = j.value("client_socket", -1);
    if (j.contains("request")) request.from_json(j["request"]);
    if (j.contains("response")) response.from_json(j["response"]);
    status = static_cast<TaskStatus>(j.value("status", 0));
    create_time = j.value("create_time", 0LL);
    assign_time = j.value("assign_time", 0LL);
    complete_time = j.value("complete_time", 0LL);
    priority = j.value("priority", 0);
    error_msg = j.value("error_msg", "");
} 
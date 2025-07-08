#pragma once
#include <etl/vector.h>
#include <string>
#include <netinet/in.h>

#define MAX_CLIENTS 512
#define MAX_JSON_SIZE 2048

struct ClientInfo {
    int socket_fd;
    sockaddr_in addr;
    bool connected;
};

// 客户端请求映射结构
struct ClientRequestMapping {
    int client_socket;
    etl::string<64> request_id;
    bool is_active;
};

using ClientList = etl::vector<ClientInfo, MAX_CLIENTS>;
using ClientRequestList = etl::vector<ClientRequestMapping, MAX_CLIENTS>;

class TaskManager; // 前向声明

// 初始化客户端管理
void client_manager_init(int listen_port);
// 添加客户端
bool client_manager_add(int socket_fd, const sockaddr_in& addr);
// 移除客户端
void client_manager_remove(int socket_fd);
// 查找客户端
ClientInfo* client_manager_find(int socket_fd);
// 获取客户端列表
ClientList* client_manager_get_list();
// 关闭所有客户端和监听socket
void client_manager_close_all();
// 事件循环，接收数据并推送到任务管理器
void client_manager_run(TaskManager* task_mgr);
// 处理所有待发送的 token - 基于 requestID 的批量策略
void client_manager_process_pending_tokens(TaskManager* task_mgr);

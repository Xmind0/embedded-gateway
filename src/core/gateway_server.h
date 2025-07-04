#pragma once
// gateway_server 已被 client_manager 取代
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
using ClientList = etl::vector<ClientInfo, MAX_CLIENTS>;

// 初始化网关服务端
bool gateway_server_init(int port);
// 关闭网关服务端
void gateway_server_close();
// 主事件循环（阻塞）
void gateway_server_run();
// 获取客户端列表
ClientList* gateway_get_client_list();
// 发送数据到指定客户端
bool gateway_send_to_client(int client_fd, const std::string& data);
// 广播数据到所有客户端
void gateway_broadcast(const std::string& data);

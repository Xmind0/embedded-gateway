#pragma once
#include <string>

// 建立 TCP 连接，返回 socket fd，失败返回 -1
typedef int socket_t;
socket_t infer_net_connect(const char* ip, int port);

// 发送 JSON 字符串到 server，返回发送字节数
int infer_net_send(socket_t sock, const std::string& json_str);

// 接收 JSON 字符串，阻塞直到收到，返回实际长度，失败返回-1
int infer_net_recv(socket_t sock, std::string& json_str);

// 关闭 socket
void infer_net_close(socket_t sock); 
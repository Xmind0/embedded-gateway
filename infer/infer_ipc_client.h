#pragma once
#include <string>

// 建立 UNIX domain socket 连接，返回 fd，失败返回 -1
typedef int ipc_socket_t;
ipc_socket_t infer_ipc_connect(const char* sock_path);

// 发送 JSON 字符串到推理引擎，返回发送字节数
int infer_ipc_send(ipc_socket_t sock, const std::string& json_str);

// 接收 JSON 字符串，阻塞直到收到，返回实际长度，失败返回-1
int infer_ipc_recv(ipc_socket_t sock, std::string& json_str);

// 关闭 socket
void infer_ipc_close(ipc_socket_t sock); 
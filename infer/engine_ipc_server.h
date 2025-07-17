#pragma once
#include <string>

// 启动 UNIX domain socket 服务端，阻塞处理推理请求
void engine_ipc_server_run(const char* sock_path); 
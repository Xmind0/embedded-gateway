#pragma once
#include <etl/vector.h>
#include <string>
#include <netinet/in.h>

#define MAX_NPU_NODES 8
#define MAX_JSON_SIZE 2048

struct NPUNodeInfo {
    int socket_fd;
    sockaddr_in addr;
    bool connected;
};
using NPUNodeList = etl::vector<NPUNodeInfo, MAX_NPU_NODES>;

// 初始化NPU节点管理
void npu_node_manager_init();
// 添加NPU节点
bool npu_add_node(const char* ip, int port);
// 关闭所有NPU节点
void npu_close_all();
// 发送数据到某个NPU节点
bool npu_send_to_node(int node_idx, const std::string& data);
// 轮询接收NPU节点数据（流式）
void npu_poll_receive();
// 获取NPU节点列表
NPUNodeList* npu_get_node_list(); 
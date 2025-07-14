#include "npu_node_manager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include "task_manager.h"
#include <etl/string.h>
#include "json_utils.h"

static NPUNodeList npu_nodes;
static TaskManager* g_task_mgr = nullptr;

// 设置全局 TaskManager 指针
void npu_set_task_manager(TaskManager* task_mgr) {
    g_task_mgr = task_mgr;
}

void npu_node_manager_init() {
    npu_nodes.clear();
}

bool npu_add_node(const char* ip, int port) {
    if (npu_nodes.size() >= MAX_NPU_NODES) return false;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        close(fd);
        return false;
    }
    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return false;
    }
    fcntl(fd, F_SETFL, O_NONBLOCK);
    npu_nodes.push_back({fd, addr, true});
    return true;
}

void npu_close_all() {
    for (auto& n : npu_nodes) {
        if (n.connected) close(n.socket_fd);
    }
    npu_nodes.clear();
}

bool npu_send_to_node(int node_idx, const std::string& data) {
    if (node_idx < 0 || node_idx >= (int)npu_nodes.size()) return false;
    auto& n = npu_nodes[node_idx];
    if (!n.connected) return false;
    ssize_t sent = send(n.socket_fd, data.c_str(), data.size(), 0);
    return sent == (ssize_t)data.size();
}

NPUNodeList* npu_get_node_list() {
    return &npu_nodes;
}

void npu_poll_receive() {
    char buf[MAX_JSON_SIZE+1];
    for (auto& n : npu_nodes) {
        if (!n.connected) continue;
        ssize_t nread = recv(n.socket_fd, buf, MAX_JSON_SIZE, MSG_DONTWAIT);
        if (nread > 0) {
            buf[nread] = 0;
            // 直接解析为ResponseMessage
            ResponseMessage resp_msg;
            if (parse_response_message(std::string(buf), resp_msg)) {
                // 可选：处理request_id/token等逻辑
                // 推送到TaskManager或转发
                if (g_task_mgr) {
                    g_task_mgr->pushResponse(n.socket_fd, buf, true);
                }
            }
        }
    }
}

void npu_forward_token(const etl::string<64>& request_id, const char* token) {
    if (g_task_mgr) {
        g_task_mgr->addToken(request_id, token);
    }
} 
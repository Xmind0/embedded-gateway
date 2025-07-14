#include "client_manager.h"
#include "task_manager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "json_utils.h"
#include "../common/data_structures.h"

static int listen_fd = -1;
static ClientList clients;
static ClientRequestList client_requests;  // 客户端请求映射列表

void client_manager_init(int listen_port) {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return;
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(listen_port);
    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        listen_fd = -1;
        return;
    }
    if (listen(listen_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        close(listen_fd);
        listen_fd = -1;
        return;
    }
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);
    clients.clear();
}

void client_manager_close_all() {
    if (listen_fd >= 0) close(listen_fd);
    for (auto& c : clients) {
        if (c.connected) close(c.socket_fd);
    }
    clients.clear();
}

ClientList* client_manager_get_list() {
    return &clients;
}

void client_manager_run(TaskManager* task_mgr) {
    fd_set readfds;
    char buf[MAX_JSON_SIZE+1];
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int maxfd = listen_fd;
        for (auto& c : clients) {
            if (c.connected) {
                FD_SET(c.socket_fd, &readfds);
                if (c.socket_fd > maxfd) maxfd = c.socket_fd;
            }
        }
        int ret = select(maxfd+1, &readfds, nullptr, nullptr, nullptr);
        if (ret < 0) continue;
        // 新连接
        if (FD_ISSET(listen_fd, &readfds)) {
            sockaddr_in cli_addr{};
            socklen_t cli_len = sizeof(cli_addr);
            int cli_fd = accept(listen_fd, (sockaddr*)&cli_addr, &cli_len);
            if (cli_fd >= 0 && clients.size() < MAX_CLIENTS) {
                fcntl(cli_fd, F_SETFL, O_NONBLOCK);
                clients.push_back({cli_fd, cli_addr, true});
            } else if (cli_fd >= 0) {
                close(cli_fd); // 超过最大连接数
            }
        }
        // 客户端数据
        for (int i = (int)clients.size()-1; i >= 0; --i) {
            auto& c = clients[i];
            if (c.connected && FD_ISSET(c.socket_fd, &readfds)) {
                ssize_t n = recv(c.socket_fd, buf, MAX_JSON_SIZE, 0);
                if (n <= 0) {
                    close(c.socket_fd);
                    clients.erase(clients.begin() + i);
                    // 清理对应的请求映射
                    for (int j = (int)client_requests.size()-1; j >= 0; --j) {
                        if (client_requests[j].client_socket == c.socket_fd) {
                            client_requests.erase(client_requests.begin() + j);
                        }
                    }
                    continue;
                }
                buf[n] = 0;
                // 直接解析为RequestMessage
                RequestMessage req_msg;
                if (parse_request_message(std::string(buf), req_msg)) {
                    // 可选：生成/补充request_id等逻辑
                    if (task_mgr) {
                        task_mgr->pushRequest(c.socket_fd, req_msg);
                    }
                }
            }
        }
        // 处理待发送的 token - 基于 requestID 的策略
        if (task_mgr) {
            client_manager_process_pending_tokens(task_mgr);
        }
    }
}

// 处理所有待发送的 token - 基于 requestID 的批量策略
void client_manager_process_pending_tokens(TaskManager* task_mgr) {
    // 遍历所有活跃的客户端请求
    for (auto& req : client_requests) {
        if (!req.is_active) continue;
        
        TokenList* list = task_mgr->getTokenList(req.request_id);
        if (!list) {
            // 如果找不到对应的 TokenList，标记为非活跃
            req.is_active = false;
            continue;
        }
        
        // 检查是否有新的 token 需要发送
        if (list->hasMoreTokens()) {
            // 发送一个 token 给对应的客户端
            const char* token = list->getNextToken();
            if (token) {
                send(req.client_socket, token, strlen(token), 0);
            }
        }
        
        // 检查是否完全结束（已发送完所有token且流已结束）
        if (list->isCompletelyFinished()) {
            req.is_active = false;
            // 清理对应的 TokenList
            task_mgr->clearTokenList(req.request_id);
        }
    }
    
    // 清理已完成的请求映射
    for (int i = (int)client_requests.size()-1; i >= 0; --i) {
        if (!client_requests[i].is_active) {
            client_requests.erase(client_requests.begin() + i);
        }
    }
}

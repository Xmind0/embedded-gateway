#include "client_manager.h"
#include "task_manager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <algorithm>

static int listen_fd = -1;
static ClientList clients;

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
                    continue;
                }
                buf[n] = 0;
                std::string json_str(buf);
                if (task_mgr) {
                    task_mgr->pushRequest(c.socket_fd, json_str);
                }
            }
        }
    }
}

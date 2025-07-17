#include "infer_ipc_client.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

ipc_socket_t infer_ipc_connect(const char* sock_path) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

int infer_ipc_send(ipc_socket_t sock, const std::string& json_str) {
    return send(sock, json_str.c_str(), json_str.size(), 0);
}

int infer_ipc_recv(ipc_socket_t sock, std::string& json_str) {
    char buf[INFER_MAX_JSON_SIZE+1];
    int n = recv(sock, buf, INFER_MAX_JSON_SIZE, 0);
    if (n > 0) {
        buf[n] = 0;
        json_str.assign(buf, n);
        return n;
    }
    return -1;
}

void infer_ipc_close(ipc_socket_t sock) {
    if (sock >= 0) close(sock);
} 
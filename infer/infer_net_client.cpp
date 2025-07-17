#include "infer_net_client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

socket_t infer_net_connect(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

int infer_net_send(socket_t sock, const std::string& json_str) {
    return send(sock, json_str.c_str(), json_str.size(), 0);
}

int infer_net_recv(socket_t sock, std::string& json_str) {
    char buf[INFER_MAX_JSON_SIZE+1];
    int n = recv(sock, buf, INFER_MAX_JSON_SIZE, 0);
    if (n > 0) {
        buf[n] = 0;
        json_str.assign(buf, n);
        return n;
    }
    return -1;
}

void infer_net_close(socket_t sock) {
    if (sock >= 0) close(sock);
} 
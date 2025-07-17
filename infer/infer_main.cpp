#include "infer_net_client.h"
#include "infer_ipc_client.h"
#include "infer_utils.h"
#include <cstdio>
#include <cstdlib>

int main() {
    printf("[INFER] Connecting to server %s:%d...\n", SERVER_IP, SERVER_PORT);
    socket_t server_sock = infer_net_connect(SERVER_IP, SERVER_PORT);
    if (server_sock < 0) { printf("[INFER] Failed to connect server!\n"); return 1; }

    printf("[INFER] Connecting to inference engine at %s...\n", INFER_ENGINE_SOCK_PATH);
    ipc_socket_t engine_sock = infer_ipc_connect(INFER_ENGINE_SOCK_PATH);
    if (engine_sock < 0) { printf("[INFER] Failed to connect engine!\n"); infer_net_close(server_sock); return 1; }

    while (1) {
        std::string req_json;
        int n = infer_net_recv(server_sock, req_json);
        if (n <= 0) { printf("[INFER] Server closed or error.\n"); break; }
        // 直接转发给推理引擎
        infer_ipc_send(engine_sock, req_json);
        std::string resp_json;
        int m = infer_ipc_recv(engine_sock, resp_json);
        if (m > 0) {
            infer_net_send(server_sock, resp_json);
        } else {
            printf("[INFER] Engine error.\n");
        }
    }
    infer_ipc_close(engine_sock);
    infer_net_close(server_sock);
    return 0;
} 
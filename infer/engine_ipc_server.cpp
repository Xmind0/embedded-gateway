#include "engine_ipc_server.h"
#include "infer_utils.h"
#include "infer_engine_api.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

void engine_ipc_server_run(const char* sock_path) {
    int listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return; }
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);
    unlink(sock_path); // 确保不会冲突
    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(listen_fd); return; }
    if (listen(listen_fd, 1) < 0) { perror("listen"); close(listen_fd); return; }
    printf("[ENGINE] Listening on %s\n", sock_path);
    while (1) {
        int cli_fd = accept(listen_fd, nullptr, nullptr);
        if (cli_fd < 0) continue;
        char buf[INFER_MAX_JSON_SIZE+1];
        int n = recv(cli_fd, buf, INFER_MAX_JSON_SIZE, 0);
        if (n > 0) {
            buf[n] = 0;
            InferRequest req;
            if (parse_infer_request(std::string(buf), req)) {
                InferResponse resp;
                resp.id = req.id;
                resp.result = mock_infer(req.prompt);
                resp.finished = true;
                std::string resp_json = dump_infer_response(resp);
                send(cli_fd, resp_json.c_str(), resp_json.size(), 0);
            }
        }
        close(cli_fd);
    }
    close(listen_fd);
} 
#include "engine_ipc_server.h"
#include "infer_utils.h"
#include <cstdio>

int main() {
    printf("[ENGINE] Starting mock inference engine...\n");
    engine_ipc_server_run(INFER_ENGINE_SOCK_PATH);
    return 0;
} 
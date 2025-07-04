#include "core/client_manager.h"
#include "core/npu_node_manager.h"
#include "core/task_manager.h"
#include <cstdio>
#include <csignal>

static bool running = true;

void handle_signal(int sig) {
    running = false;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    int port = 9000; // 客户端监听端口
    client_manager_init(port);
    npu_node_manager_init();
    TaskManager task_mgr;
    // task_mgr.start(); // 如需多线程任务处理可启用
    // 示例：添加一个NPU节点
    // npu_add_node("192.168.1.100", 10000);

    printf("[INFO] Client manager started on port %d\n", port);
    while (running) {
        client_manager_run(&task_mgr); // 事件循环，接收数据并推送到任务队列
        npu_poll_receive();   // 轮询NPU节点数据
        // 可在此处处理任务响应等逻辑
    }
    client_manager_close_all();
    npu_close_all();
    printf("[INFO] Server stopped.\n");
    return 0;
}

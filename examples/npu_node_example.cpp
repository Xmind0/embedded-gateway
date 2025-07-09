#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "../src/core/message_handler.h"

class NPUNodeExample {
private:
    int sock_fd;
    std::string gateway_ip;
    int gateway_port;
    std::string node_id;
    bool running;
    
public:
    NPUNodeExample(const std::string& ip, int port, const std::string& id) 
        : gateway_ip(ip), gateway_port(port), node_id(id), running(false), sock_fd(-1) {}
    
    ~NPUNodeExample() {
        if (sock_fd >= 0) {
            close(sock_fd);
        }
    }
    
    bool connect() {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(gateway_port);
        server_addr.sin_addr.s_addr = inet_addr(gateway_ip.c_str());
        
        if (::connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Failed to connect to gateway" << std::endl;
            return false;
        }
        
        std::cout << "Connected to gateway " << gateway_ip << ":" << gateway_port << std::endl;
        return true;
    }
    
    void start() {
        running = true;
        
        // 发送节点状态
        send_status();
        
        // 启动心跳线程
        std::thread heartbeat_thread(&NPUNodeExample::heartbeat_loop, this);
        heartbeat_thread.detach();
        
        // 主消息处理循环
        message_loop();
    }
    
    void stop() {
        running = false;
    }
    
private:
    void send_status() {
        std::string status_msg = MessageHandler::build_status(node_id, true, 0.5f);
        send(sock_fd, status_msg.c_str(), status_msg.length(), 0);
    }
    
    void heartbeat_loop() {
        while (running) {
            std::string heartbeat_msg = MessageHandler::build_heartbeat();
            send(sock_fd, heartbeat_msg.c_str(), heartbeat_msg.length(), 0);
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }
    
    void message_loop() {
        char buffer[4096];
        std::string message_data;
        
        while (running) {
            int bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                std::cout << "Connection closed by gateway" << std::endl;
                break;
            }
            
            buffer[bytes_received] = '\0';
            message_data += buffer;
            
            // 尝试解析完整的JSON消息
            Message msg;
            if (MessageHandler::parse_message(message_data, msg)) {
                handle_message(msg);
                message_data.clear();
            }
        }
    }
    
    void handle_message(const Message& msg) {
        switch (msg.type) {
            case MessageType::TASK:
                handle_inference_task(msg);
                break;
                
            case MessageType::HEARTBEAT:
                std::cout << "Received heartbeat from gateway" << std::endl;
                break;
                
            default:
                std::cout << "Unknown message type" << std::endl;
                break;
        }
    }
    
    void handle_inference_task(const Message& msg) {
        std::cout << "Received inference task: " << msg.id << std::endl;
        std::cout << "Model: " << msg.model << std::endl;
        std::cout << "Prompt: " << msg.prompt << std::endl;
        
        // 模拟推理过程
        if (msg.stream) {
            // 流式输出
            std::string result = "你好！人工智能是计算机科学的一个分支，它致力于创建能够执行通常需要人类智能的任务的机器。";
            
            for (size_t i = 0; i < result.length(); i += 2) {
                std::string token = result.substr(i, 2);
                std::string response = MessageHandler::build_stream_response(
                    msg.id, msg.client_socket, token, (i + 2 >= result.length())
                );
                
                send(sock_fd, response.c_str(), response.length(), 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            // 非流式输出
            std::string result = "你好！人工智能是计算机科学的一个分支，它致力于创建能够执行通常需要人类智能的任务的机器。";
            std::string response = MessageHandler::build_response(msg.id, result, true);
            send(sock_fd, response.c_str(), response.length(), 0);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <gateway_ip> <gateway_port> <node_id>" << std::endl;
        return 1;
    }
    
    std::string gateway_ip = argv[1];
    int gateway_port = std::stoi(argv[2]);
    std::string node_id = argv[3];
    
    NPUNodeExample node(gateway_ip, gateway_port, node_id);
    
    if (!node.connect()) {
        return 1;
    }
    
    std::cout << "NPU Node " << node_id << " started" << std::endl;
    node.start();
    
    return 0;
} 
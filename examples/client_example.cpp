#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../src/core/message_handler.h"

class ClientExample {
private:
    int sock_fd;
    std::string server_ip;
    int server_port;
    
public:
    ClientExample(const std::string& ip, int port) : server_ip(ip), server_port(port), sock_fd(-1) {}
    
    ~ClientExample() {
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
        server_addr.sin_port = htons(server_port);
        server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
        
        if (::connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Failed to connect to server" << std::endl;
            return false;
        }
        
        std::cout << "Connected to server " << server_ip << ":" << server_port << std::endl;
        return true;
    }
    
    bool send_request(const std::string& model, const std::string& prompt, int max_tokens = 1000, bool stream = true) {
        std::string request_id = "req_" + std::to_string(time(nullptr));
        std::string json_request = MessageHandler::build_request(request_id, model, prompt, max_tokens, stream);
        
        std::cout << "Sending request: " << json_request << std::endl;
        
        if (send(sock_fd, json_request.c_str(), json_request.length(), 0) < 0) {
            std::cerr << "Failed to send request" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void receive_response() {
        char buffer[4096];
        std::string response_data;
        
        while (true) {
            int bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                std::cout << "Connection closed by server" << std::endl;
                break;
            }
            
            buffer[bytes_received] = '\0';
            response_data += buffer;
            
            // 尝试解析完整的JSON消息
            Message msg;
            if (MessageHandler::parse_message(response_data, msg)) {
                handle_message(msg);
                response_data.clear(); // 清空已处理的数据
            }
        }
    }
    
private:
    void handle_message(const Message& msg) {
        switch (msg.type) {
            case MessageType::RESPONSE:
                if (msg.finished) {
                    std::cout << "Final result: " << msg.result << std::endl;
                } else {
                    std::cout << "Token: " << msg.token << std::flush;
                }
                break;
                
            case MessageType::ERROR:
                std::cerr << "Error: " << msg.message << std::endl;
                break;
                
            case MessageType::HEARTBEAT:
                std::cout << "Received heartbeat" << std::endl;
                break;
                
            default:
                std::cout << "Unknown message type" << std::endl;
                break;
        }
    }
};

int main() {
    ClientExample client("127.0.0.1", 8080);
    
    if (!client.connect()) {
        return 1;
    }
    
    // 发送推理请求
    std::string model = "llama2-7b";
    std::string prompt = "你好，请介绍一下人工智能";
    
    if (!client.send_request(model, prompt)) {
        return 1;
    }
    
    // 接收响应
    client.receive_response();
    
    return 0;
} 
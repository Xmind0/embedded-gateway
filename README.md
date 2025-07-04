# C++ Socket Gateway Server

一个高性能的C++ Socket网关服务器，支持多客户端连接、任务队列管理和推理服务器分配。

## 项目特性

- **高并发支持**: 支持最多1000个并发客户端连接
- **模块化设计**: 客户端管理、任务管理、推理连接器分离
- **任务队列**: 自定义静态队列，避免动态内存分配
- **推理服务器分配**: 支持多个推理服务器，多种分配策略
- **异常处理**: 完善的错误处理和超时机制
- **线程安全**: 多线程环境下的安全操作
- **内存安全**: 避免内存泄漏和悬空指针

## 项目结构

```
gateway/
├── CMakeLists.txt                 # 主CMake文件
├── src/                           # 源代码
│   ├── main.cpp                   # 主程序入口
│   ├── common/                    # 公共组件
│   │   ├── data_structures.h      # 数据结构（消息类、队列、缓存等）
│   │   ├── noncopyable.h          # 不可拷贝基类
│   │   └── thread_safe_queue.h    # 线程安全队列
│   ├── core/                      # 核心模块
│   │   ├── gateway_server.h/cpp   # 网关服务器主类
│   │   ├── client_manager.h/cpp   # 客户端管理器
│   │   ├── backend_connector.h/cpp # 后端连接器
│   │   └── task_manager.h/cpp     # 任务管理器
│   ├── utils/                     # 工具类
│   │   ├── logger.h/cpp           # 日志工具
│   │   └── network_utils.h/cpp    # 网络工具
│   └── main.cpp                   # 主程序入口
├── build/                         # 构建目录
└── README.md                      # 项目说明
```

## 核心功能

### 1. 客户端管理器 (ClientManager)
- 监听指定端口，接受客户端连接
- 为每个客户端创建独立处理线程
- 将客户端消息存储到消息缓存队列
- 支持最大1000个并发客户端

### 2. 消息缓存 (ClientMessageCache)
- 使用自定义队列存储 `RequestMessage*` 指针
- 支持消息的添加、获取和清理
- 自动管理内存，防止内存泄漏

### 3. 任务管理器 (TaskManager)
- 处理客户端请求
- 生成响应数据
- 管理响应追踪器

### 4. 响应追踪器 (ResponseTracker)
- 使用 `std::unordered_map` 追踪每个客户端的响应
- 支持响应的设置、获取和移除

### 5. 推理服务器连接器 (InferenceConnector)
- 连接推理服务器
- 发送请求
- 接收响应
- 处理错误

## 编译方法

### 环境要求
- CMake 3.10+
- C++14 编译器 (GCC 5+, Clang 3.5+, MSVC 2015+)
- Linux/Unix 系统 (支持 POSIX socket)

### 编译步骤

1. **创建构建目录**
```bash
mkdir build
cd build
```

2. **配置项目**
```bash
cmake ..
```

3. **编译项目**
```bash
make -j$(nproc)
```

4. **安装 (可选)**
```bash
make install
```

### 编译产物

编译后会生成两个可执行文件：

- `gateway_server` - 主网关服务器程序
- `mock_inference_server` - 模拟推理服务器（用于测试）
- `gateway_tests` - 测试程序

## 运行方法

### 启动网关服务器
```bash
./bin/gateway_server
```

服务器默认监听8080端口。

### 运行测试
```bash
./bin/gateway_tests
```

## 测试说明

### 测试覆盖范围

1. **ClientManager 测试**
   - 基本功能测试（启动/停止）
   - 客户端连接测试
   - 消息处理测试
   - 多客户端并发测试

2. **GatewayServer 测试**
   - 服务器启动/停止测试
   - 客户端通信测试
   - 消息流测试
   - 并发客户端测试

3. **TaskManager 测试**
   - 基本功能测试
   - 请求处理测试
   - 多请求处理测试
   - 响应追踪测试
   - 并发处理测试

4. **InferenceConnector 测试**
   - 连接测试
   - 请求发送测试
   - 响应接收测试
   - 错误处理测试

### 测试方法

1. **运行所有测试**
```bash
./bin/gateway_tests
```

2. **单独运行特定测试**
```bash
# 客户端管理器测试
./bin/test_client_manager

# 网关服务器测试
./bin/test_gateway_server

# 任务管理器测试
./bin/test_task_manager

# 推理服务器测试
./bin/test_inference_connector
```

### 测试客户端示例

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) >= 0) {
        const char* message = "Hello, Gateway Server!";
        send(client_socket, message, strlen(message), 0);
        close(client_socket);
    }
    
    return 0;
}
```

## 架构设计

### 线程模型
- **主线程**: 启动和协调各个组件
- **接受线程**: 监听新客户端连接
- **客户端线程**: 每个客户端一个独立线程处理数据
- **消息处理线程**: 处理消息缓存中的消息
- **工作线程**: 处理任务和生成响应

### 数据流
```
客户端连接 → ClientManager → 消息缓存 → GatewayServer → TaskManager → 任务队列 → InferenceConnector → 推理服务器
                                                                                    ↓
推理服务器 → InferenceConnector → TaskManager → 响应追踪器 → ClientManager → 客户端
```

### 内存管理
- 使用智能指针和RAII管理资源
- 自定义队列避免频繁动态分配
- 响应追踪器使用标准库容器
- 自动清理断开的客户端连接

## 性能特性

- 支持1000个并发客户端
- 每个客户端消息队列容量100条
- 响应追踪器容量1000个客户端
- 线程安全的消息处理
- 高效的内存管理

## 扩展建议

1. **添加配置文件支持**
2. **实现负载均衡**
3. **添加监控和统计功能**
4. **支持SSL/TLS加密**
5. **实现集群模式**
6. **添加数据库持久化**

## 故障排除

### 常见问题

1. **端口被占用**
   - 修改 `main.cpp` 中的端口号
   - 或使用 `netstat -tulpn | grep :8080` 检查端口占用

2. **编译错误**
   - 确保C++14支持
   - 检查依赖库是否正确安装

3. **运行时错误**
   - 检查防火墙设置
   - 确保有足够的文件描述符限制

### 调试方法

1. **启用详细日志**
   - 修改 `logger.h` 中的日志级别

2. **使用GDB调试**
```bash
gdb ./bin/gateway_server
```

3. **性能分析**
```bash
valgrind --tool=memcheck ./bin/gateway_server
```

## 许可证

本项目采用 MIT 许可证。

## 贡献

欢迎提交Issue和Pull Request来改进项目。

# Gateway Minimal Example

## 项目结构

- src/core/client_manager.h/.cpp         # 客户端管理类
- src/core/task_manager.h/.cpp           # 任务管理类
- src/core/inference_connector.h/.cpp    # 推理节点管理类（InferenceNodeManager）
- src/core/gateway_server.h/.cpp         # 顶层网关服务器
- src/utils/logger.h/.cpp                # 简单日志工具
- src/main.cpp                           # 启动入口
- CMakeLists.txt                         # 构建脚本

## 编译

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## 运行

```sh
./gateway
```

## 说明
- 仅为极简分层骨架，所有 socket 通信和任务流转均为基础实现。
- 可在 main.cpp 入口自定义端口和启动参数。 
# C++ Socket Gateway Server

一个的C++ Socket网关服务器，支持多客户端连接、任务队列管理和推理服务器分配。（不是一个完整的项目，无法使用，仅学习使用（全部AI生成）），每次git提交都会生成新的功能（供学习使用）

## 新增功能：精简JSON通信协议

本项目新增了精简的JSON通信协议，用于客户端、网关服务端和NPU推理节点之间的通信。

### 协议特点
- **极简设计**：只保留必要字段，适合嵌入式环境
- **统一格式**：所有消息都有统一的`type`和`id`字段
- **流式支持**：支持token流式输出
- **错误处理**：统一的错误响应格式
- **内存友好**：字段少，解析快


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
│   │   ├── task_manager.h/cpp     # 任务管理器
│   │   ├── json_utils.h/cpp       # JSON工具类
│   │   └── message_handler.h/cpp  # 消息处理类
│   ├── utils/                     # 工具类
│   │   ├── logger.h/cpp           # 日志工具
│   │   └── network_utils.h/cpp    # 网络工具
│   └── main.cpp                   # 主程序入口
├── examples/                      # 示例代码
│   ├── client_example.cpp         # 客户端示例
│   └── npu_node_example.cpp       # NPU节点示例
├── docs/                          # 文档
│   ├── json_protocol.md           # JSON协议文档
│   └── usage_guide.md             # 使用指南
├── build/                         # 构建目录
└── README.md                      # 项目说明

## 快速开始

### 编译项目
```bash
mkdir build
cd build
cmake ..
make
```

### 运行示例
```bash
# 运行网关服务端
./bin/gateway_server

# 运行NPU节点
./bin/npu_node_example 127.0.0.1 8080 npu_001

# 运行客户端示例
./bin/client_example
```

## 文档

- [JSON通信协议](docs/json_protocol.md) - 详细的协议规范
- [使用指南](docs/usage_guide.md) - API使用和示例代码
```


# JSON通信协议使用指南

## 概述

本项目实现了一个精简的JSON通信协议，用于客户端、网关服务端和NPU推理节点之间的通信。

## 协议特点

- **极简设计**：只保留必要字段，适合嵌入式环境
- **统一格式**：所有消息都有统一的`type`和`id`字段
- **流式支持**：支持token流式输出
- **错误处理**：统一的错误响应格式
- **内存友好**：字段少，解析快

## 消息类型

### 1. 客户端请求 (request)
```json
{
  "type": "request",
  "id": "req_123",
  "model": "llama2-7b",
  "prompt": "你好，请介绍一下人工智能",
  "max_tokens": 1000,
  "stream": true
}
```

### 2. 服务端任务 (task)
```json
{
  "type": "task",
  "id": "req_123",
  "client_socket": 12345,
  "model": "llama2-7b",
  "prompt": "你好，请介绍一下人工智能",
  "max_tokens": 1000,
  "stream": true
}
```

### 3. 推理响应 (response)
```json
{
  "type": "response",
  "id": "req_123",
  "token": "你",
  "finished": false
}
```

### 4. 错误消息 (error)
```json
{
  "type": "error",
  "id": "req_123",
  "message": "模型不存在"
}
```

### 5. 心跳消息 (heartbeat)
```json
{
  "type": "heartbeat"
}
```

### 6. 状态消息 (status)
```json
{
  "type": "status",
  "node_id": "npu_001",
  "available": true,
  "load": 0.75
}
```

## 使用示例

### 编译项目
```bash
mkdir build
cd build
cmake ..
make
```

### 运行网关服务端
```bash
./bin/gateway_server
```

### 运行NPU节点
```bash
./bin/npu_node_example 127.0.0.1 8080 npu_001
```

### 运行客户端示例
```bash
./bin/client_example
```

## API使用

### 消息解析
```cpp
#include "src/core/message_handler.h"

std::string json_str = "...";
Message msg;
if (MessageHandler::parse_message(json_str, msg)) {
    switch (msg.type) {
        case MessageType::REQUEST:
            // 处理请求
            break;
        case MessageType::RESPONSE:
            // 处理响应
            break;
    }
}
```

### 消息构建
```cpp
// 构建请求
std::string request = MessageHandler::build_request("req_123", "llama2-7b", "你好", 1000, true);

// 构建响应
std::string response = MessageHandler::build_response("req_123", "结果内容", true);

// 构建流式响应
std::string stream_response = MessageHandler::build_stream_response("req_123", 12345, "token", false);

// 构建错误
std::string error = MessageHandler::build_error("req_123", "错误信息");
```

## 字段说明

| 字段名 | 类型 | 说明 |
|--------|------|------|
| type | string | 消息类型 |
| id | string | 请求唯一标识符 |
| model | string | 模型名称 |
| prompt | string | 输入提示 |
| max_tokens | int | 最大输出token数 |
| stream | bool | 是否流式输出 |
| client_socket | int | 客户端socket ID |
| token | string | 流式输出的单个token |
| result | string | 完整输出结果 |
| finished | bool | 是否完成 |
| message | string | 错误信息 |
| node_id | string | 节点ID |
| available | bool | 节点是否可用 |
| load | float | 节点负载 |

## 注意事项

1. **内存管理**：在嵌入式环境中，避免使用动态内存分配
2. **消息完整性**：确保JSON消息的完整性，避免解析错误
3. **错误处理**：始终检查消息解析和发送的返回值
4. **并发安全**：在多线程环境中使用适当的同步机制
5. **网络超时**：设置合适的网络超时时间，避免连接阻塞

## 扩展建议

1. **添加认证**：在消息中添加认证字段
2. **压缩支持**：对大型消息进行压缩
3. **加密传输**：对敏感数据进行加密
4. **版本控制**：添加协议版本字段
5. **监控指标**：添加性能监控字段 
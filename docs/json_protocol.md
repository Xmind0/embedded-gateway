# JSON通信协议设计

## 精简版JSON通信格式

### 1. 客户端 → 服务端请求格式

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

### 2. 服务端 → NPU推理节点请求格式

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

### 3. NPU推理节点 → 服务端响应格式

#### 3.1 流式响应
```json
{
  "type": "response",
  "id": "req_123",
  "client_socket": 12345,
  "token": "你",
  "finished": false
}
```

#### 3.2 完成响应
```json
{
  "type": "response",
  "id": "req_123",
  "client_socket": 12345,
  "result": "你好！人工智能是计算机科学的一个分支...",
  "finished": true
}
```

#### 3.3 错误响应
```json
{
  "type": "error",
  "id": "req_123",
  "client_socket": 12345,
  "message": "模型不存在"
}
```

### 4. 服务端 → 客户端响应格式

#### 4.1 流式响应
```json
{
  "type": "response",
  "id": "req_123",
  "token": "你",
  "finished": false
}
```

#### 4.2 完成响应
```json
{
  "type": "response",
  "id": "req_123",
  "result": "你好！人工智能是计算机科学的一个分支...",
  "finished": true
}
```

#### 4.3 错误响应
```json
{
  "type": "error",
  "id": "req_123",
  "message": "模型不存在"
}
```

### 5. 系统消息格式

#### 5.1 心跳
```json
{
  "type": "heartbeat"
}
```

#### 5.2 节点状态
```json
{
  "type": "status",
  "node_id": "npu_001",
  "available": true,
  "load": 0.75
}
```

## 设计特点

1. **极简字段**：只保留必要字段，去掉`timestamp`、`client_info`等
2. **统一ID**：所有消息都用简单的`id`字段
3. **类型明确**：`type`字段区分消息类型
4. **流式支持**：用`finished`字段标记流结束
5. **错误处理**：简单的错误消息格式
6. **内存友好**：字段少，解析快，适合嵌入式环境

## 字段说明

- `type`: 消息类型 ("request", "task", "response", "error", "heartbeat", "status")
- `id`: 请求唯一标识符
- `model`: 模型名称
- `prompt`: 输入提示
- `max_tokens`: 最大输出token数
- `stream`: 是否流式输出
- `client_socket`: 客户端socket ID
- `token`: 流式输出的单个token
- `result`: 完整输出结果
- `finished`: 是否完成
- `message`: 错误信息
- `node_id`: 节点ID
- `available`: 节点是否可用
- `load`: 节点负载 
# C++ Socket Gateway Server

一个的C++ Socket网关服务器，支持多客户端连接、任务队列管理和推理服务器分配。（不是一个完整的项目，无法使用，仅学习使用（全部AI生成）），每次git提交都会生成新的功能（供学习使用）


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


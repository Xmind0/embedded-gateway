CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2
CXXFLAGS_EMBEDDED = -std=c++17 -pthread -Wall -Wextra -O2 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections
LDFLAGS_EMBEDDED = -Wl,--gc-sections -Wl,--strip-all
INCLUDES = -Isrc

# 源文件
GATEWAY_SOURCES = src/main.cpp \
                  src/core/gateway_server.cpp \
                  src/core/client_manager.cpp \
                  src/core/task_manager.cpp \
                  src/core/inference_connector.cpp \
                  src/core/response_manager.cpp \
                  src/core/backend_connector.cpp \
                  src/utils/logger.cpp

TEST_SOURCES = src/tests/test_gateway_server.cpp \
               src/tests/test_client_manager.cpp \
               src/tests/test_task_manager.cpp \
               src/tests/test_inference_connector.cpp \
               src/tests/test_response_manager.cpp

# 目标文件
GATEWAY_TARGET = gateway_server
AUTO_INFERENCE_TARGET = auto_inference_server
AUTO_CLIENT_TARGET = auto_client
MOCK_INFERENCE_TARGET = mock_inference_server

# 默认目标
all: $(GATEWAY_TARGET) $(AUTO_INFERENCE_TARGET) $(AUTO_CLIENT_TARGET) $(MOCK_INFERENCE_TARGET)

# 嵌入式Linux目标
embedded: CXXFLAGS = $(CXXFLAGS_EMBEDDED)
embedded: LDFLAGS = $(LDFLAGS_EMBEDDED)
embedded: $(GATEWAY_TARGET) $(AUTO_INFERENCE_TARGET) $(AUTO_CLIENT_TARGET) $(MOCK_INFERENCE_TARGET)

# 网关服务器
$(GATEWAY_TARGET): $(GATEWAY_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^

# 自动连接推理服务器
$(AUTO_INFERENCE_TARGET): src/tests/auto_connect_inference_server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $<

# 自动连接客户端
$(AUTO_CLIENT_TARGET): src/tests/auto_connect_client.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $<

# Mock推理服务器
$(MOCK_INFERENCE_TARGET): src/tests/mock_inference_server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $<

# 清理
clean:
	rm -f $(GATEWAY_TARGET) $(AUTO_INFERENCE_TARGET) $(AUTO_CLIENT_TARGET) $(MOCK_INFERENCE_TARGET)

# 运行示例
run-gateway: $(GATEWAY_TARGET)
	./$(GATEWAY_TARGET)

run-inference: $(AUTO_INFERENCE_TARGET)
	./$(AUTO_INFERENCE_TARGET)

run-client: $(AUTO_CLIENT_TARGET)
	./$(AUTO_CLIENT_TARGET)

run-mock: $(MOCK_INFERENCE_TARGET)
	./$(MOCK_INFERENCE_TARGET) 9090

# 显示文件大小
size: $(GATEWAY_TARGET) $(AUTO_INFERENCE_TARGET) $(AUTO_CLIENT_TARGET) $(MOCK_INFERENCE_TARGET)
	@echo "可执行文件大小："
	@ls -lh $(GATEWAY_TARGET) $(AUTO_INFERENCE_TARGET) $(AUTO_CLIENT_TARGET) $(MOCK_INFERENCE_TARGET)

# 帮助
help:
	@echo "可用的目标："
	@echo "  all              - 编译所有程序（标准版本）"
	@echo "  embedded         - 编译所有程序（嵌入式Linux优化版本）"
	@echo "  gateway_server   - 编译网关服务器"
	@echo "  auto_inference_server - 编译自动连接推理服务器"
	@echo "  auto_client      - 编译自动连接客户端"
	@echo "  mock_inference_server - 编译Mock推理服务器"
	@echo "  clean            - 清理编译文件"
	@echo "  run-gateway      - 运行网关服务器"
	@echo "  run-inference    - 运行自动连接推理服务器"
	@echo "  run-client       - 运行自动连接客户端"
	@echo "  run-mock         - 运行Mock推理服务器"
	@echo "  size             - 显示可执行文件大小"
	@echo "  help             - 显示此帮助信息"
	@echo
	@echo "嵌入式Linux优化选项："
	@echo "  -fno-exceptions  - 禁用异常处理"
	@echo "  -fno-rtti        - 禁用RTTI"
	@echo "  -ffunction-sections - 每个函数放入单独的段"
	@echo "  -fdata-sections  - 每个数据项放入单独的段"
	@echo "  --gc-sections    - 链接时垃圾回收"
	@echo "  --strip-all      - 移除所有符号"

.PHONY: all embedded clean run-gateway run-inference run-client run-mock size help 
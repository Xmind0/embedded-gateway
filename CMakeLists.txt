cmake_minimum_required(VERSION 3.10)
project(GatewayServer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 查找必要的包
find_package(Threads REQUIRED)

# 包含目录
include_directories(src)

# 源文件
set(COMMON_SOURCES
    src/common/data_structures.h
    src/common/noncopyable.h
    src/common/thread_safe_queue.h
)

set(UTILS_SOURCES
    src/utils/logger.h
    src/utils/logger.cpp
)

set(CORE_SOURCES
    src/core/client_manager.h
    src/core/client_manager.cpp
    src/core/gateway_server.h
    src/core/gateway_server.cpp
    src/core/task_manager.h
    src/core/task_manager.cpp
    src/core/task_cache.h
    src/core/task_cache.cpp
    src/core/task_queue.h
    src/core/task_queue.cpp
    src/core/task_context.h
    src/core/backend_connector.h
    src/core/backend_connector.cpp
    src/core/inference_connector.h
    src/core/inference_connector.cpp
    src/core/json_utils.h
    src/core/json_utils.cpp
    src/core/message_handler.h
    src/core/message_handler.cpp
)

# 网关服务器可执行文件
add_executable(gateway_server
    src/main.cpp
    ${COMMON_SOURCES}
    ${UTILS_SOURCES}
    ${CORE_SOURCES}
)

# 链接线程库
target_link_libraries(gateway_server Threads::Threads)

# 模拟推理服务器
add_executable(mock_inference_server
    src/tests/mock_inference_server.cpp
)

target_link_libraries(mock_inference_server Threads::Threads)

# 示例程序
add_executable(client_example
    examples/client_example.cpp
    ${CORE_SOURCES}
)

add_executable(npu_node_example
    examples/npu_node_example.cpp
    ${CORE_SOURCES}
)

target_link_libraries(client_example Threads::Threads)
target_link_libraries(npu_node_example Threads::Threads)

# 测试程序
set(TEST_SOURCES
    src/tests/test_client_manager.cpp
    src/tests/test_gateway_server.cpp
    src/tests/test_task_manager.cpp
    src/tests/test_inference_connector.cpp
)

# 如果测试文件存在，则创建测试可执行文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_client_manager.cpp")
    add_executable(gateway_tests
        ${TEST_SOURCES}
        ${COMMON_SOURCES}
        ${UTILS_SOURCES}
        ${CORE_SOURCES}
    )
    
    target_link_libraries(gateway_tests Threads::Threads)
endif()

# 安装规则
install(TARGETS gateway_server mock_inference_server client_example npu_node_example
    RUNTIME DESTINATION bin
)

# 设置输出目录
set_target_properties(gateway_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(mock_inference_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(client_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(npu_node_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(TARGET gateway_tests)
    set_target_properties(gateway_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif() 
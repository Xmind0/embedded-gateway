#pragma once
#include <string>

// mock 推理 API，输入 prompt，返回 result
inline std::string mock_infer(const std::string& prompt) {
    return "[MOCK_RESULT] " + prompt;
} 
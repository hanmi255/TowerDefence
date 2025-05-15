# 设置 CMP0111 策略
cmake_policy(SET CMP0111 NEW)

# 定义cJSON库
add_library(cJSON OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON.c
)

# 设置头文件目录
target_include_directories(cJSON
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
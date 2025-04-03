# 收集 bullet 文件夹下的所有头文件
file(GLOB BULLET_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(bullet INTERFACE)

# 设置包含目录
target_include_directories(bullet INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 bullet 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(bullet PROPERTIES INTERFACE_SOURCES "${BULLET_HEADERS}")
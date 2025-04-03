# 收集 enemy 文件夹下的所有头文件
file(GLOB ENEMY_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(enemy INTERFACE)

# 设置包含目录
target_include_directories(enemy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 enemy 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(enemy PROPERTIES INTERFACE_SOURCES "${ENEMY_HEADERS}")
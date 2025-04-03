# 收集 manager 文件夹下的所有头文件
file(GLOB MANAGER_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(manager INTERFACE)

# 设置包含目录
target_include_directories(manager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 manager 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(manager PROPERTIES INTERFACE_SOURCES "${MANAGER_HEADERS}")
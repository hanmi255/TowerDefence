# 收集 basic 文件夹下的所有头文件
file(GLOB BASIC_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(basic INTERFACE)

# 设置包含目录
target_include_directories(basic INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 basic 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(basic PROPERTIES INTERFACE_SOURCES "${BASIC_HEADERS}")
# 收集 util 文件夹下的所有头文件
file(GLOB UTIL_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(util INTERFACE)

# 设置包含目录
target_include_directories(util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 util 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(util PROPERTIES INTERFACE_SOURCES "${UTIL_HEADERS}")
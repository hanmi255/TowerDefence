# 收集 tower 文件夹下的所有头文件
file(GLOB TOWER_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(tower INTERFACE)

# 设置包含目录
target_include_directories(tower INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 tower 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(tower PROPERTIES INTERFACE_SOURCES "${TOWER_HEADERS}")
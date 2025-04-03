# 收集 ui 文件夹下的所有头文件
file(GLOB UI_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(ui INTERFACE)

# 添加子目录
add_subdirectory(panel)

# 链接 panel 模块S
target_link_libraries(ui INTERFACE panel)

# 设置包含目录
target_include_directories(ui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 ui 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(ui PROPERTIES INTERFACE_SOURCES "${UI_HEADERS}")
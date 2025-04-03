# 收集 panel 文件夹下的所有头文件
file(GLOB PANEL_HEADERS "*.hpp" "*.h")

# 创建接口库（仅头文件）
add_library(panel INTERFACE)

# 设置包含目录
target_include_directories(panel INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 将 panel 文件夹下的所有头文件设置为接口库的源文件
set_target_properties(panel PROPERTIES INTERFACE_SOURCES "${PANEL_HEADERS}")
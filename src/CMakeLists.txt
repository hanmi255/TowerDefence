# 列出所有源文件
set(SOURCES
    main.cpp
)
# 为Qt模块启用自动化处理
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 添加子目录
add_subdirectory(basic)
add_subdirectory(bullet)
add_subdirectory(enemy)
add_subdirectory(manager)
add_subdirectory(tower)
add_subdirectory(ui)
add_subdirectory(util)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 配置Qt模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# 为MSVC设置编译和链接选项
if(MSVC)
    # 设置全局运行时库为动态链接
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # 为主目标启用多处理器编译和添加调试信息
    target_compile_options(${PROJECT_NAME} PRIVATE
        /MP # 多处理器编译
        /wd4244 /wd4267 # 禁用常见警告
        $<$<CONFIG:Debug>:/Zi> # 调试信息
    )

    # 添加忽略默认库的链接选项，解决冲突
    target_link_options(${PROJECT_NAME} PRIVATE
        "/NODEFAULTLIB:libcmt.lib"
        "$<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>"
    )
endif()

# 链接Qt模块
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

# 链接第三方库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    cJSON
    SDL2main
    SDL2
    SDL2_gfx
    SDL2_image
    SDL2_mixer
    SDL2_ttf
	boost_lib
	ElaWidgetTools
)

# 链接基本库和模块库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    basic
    bullet
    enemy
    manager
    tower
    ui
    util
)

# 复制资源文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
    COMMENT "Copying resource files to output directory"
)

# 如果是Windows，复制DLL到输出目录
if(WIN32 AND DEFINED SDL2_DLLS_INFO)
    foreach(SDL2_DLL ${SDL2_DLLS_INFO})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL2_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endforeach()
endif()

# 输出诊断信息
message(STATUS "项目源代码目录: ${CMAKE_SOURCE_DIR}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
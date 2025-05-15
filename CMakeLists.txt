# 设置cmake标准
cmake_minimum_required(VERSION 3.14)

project(TowerDefence LANGUAGES C CXX)

# 设置全局 CMP0167 策略
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 为 MSVC 编译器添加特殊选项
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# vcpkg集成
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# 如果没有设置vcpkg.json的manifest模式，可以选择性地添加以下内容
# set(VCPKG_MANIFEST_MODE ON)
# set(VCPKG_MANIFEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 确保编译输出的目录结构
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 1、预先收集所有SDL2相关的DLL
# 2、预先收集ElaWidgetTools的DLL
if(WIN32)
    set(SDL2_DLLS
        "${CMAKE_SOURCE_DIR}/third_party/SDL2/lib/x64/SDL2.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_image/lib/x64/SDL2_image.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_mixer/lib/x64/SDL2_mixer.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_ttf/lib/x64/SDL2_ttf.dll"
    )
	set(ElaWidgetTools_DLL
        "${CMAKE_SOURCE_DIR}/third_party/ElaWidgetTools/lib/ElaWidgetToolsd.dll"
    )

    # 将DLL信息传递给子目录
    set(SDL2_DLLS_INFO ${SDL2_DLLS} CACHE INTERNAL "SDL2 DLLs to copy")
	set(ElaWidgetTools_DLL_INFO ${ElaWidgetTools_DLL} CACHE INTERNAL "ElaWidgetTools DLL to copy")
endif()

# 添加第三方子目录
add_subdirectory(third_party/cJSON)
add_subdirectory(third_party/SDL2)
add_subdirectory(third_party/SDL2_gfx)
add_subdirectory(third_party/SDL2_image)
add_subdirectory(third_party/SDL2_mixer)
add_subdirectory(third_party/SDL2_ttf)
add_subdirectory(third_party/boost)
add_subdirectory(third_party/ElaWidgetTools)

# 添加源码目录
add_subdirectory(src)
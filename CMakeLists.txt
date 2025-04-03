# 设置cmake标准
cmake_minimum_required(VERSION 3.10)

# 设置全局 CMP0111 策略
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()

project(TowerDefence LANGUAGES C CXX)

# 设置C/C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保编译输出的目录结构
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 预先收集所有SDL2相关的DLL
if(WIN32)
    set(SDL2_DLLS 
        "${CMAKE_SOURCE_DIR}/third_party/SDL2/lib/x64/SDL2.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_image/lib/x64/SDL2_image.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_mixer/lib/x64/SDL2_mixer.dll"
        "${CMAKE_SOURCE_DIR}/third_party/SDL2_ttf/lib/x64/SDL2_ttf.dll"
    )
    # 将DLL信息传递给子目录
    set(SDL2_DLLS_INFO ${SDL2_DLLS} CACHE INTERNAL "SDL2 DLLs to copy")
endif()

# 添加子目录
add_subdirectory(third_party/SDL2)
add_subdirectory(third_party/SDL2_gfx)
add_subdirectory(third_party/SDL2_image)
add_subdirectory(third_party/SDL2_mixer)
add_subdirectory(third_party/SDL2_ttf)

# 添加源码目录
add_subdirectory(src)
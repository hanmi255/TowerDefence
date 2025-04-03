# 设置 CMP0111 策略
cmake_policy(SET CMP0111 NEW)

# 定义SDL2_ttf库
add_library(SDL2_ttf SHARED IMPORTED GLOBAL)

# 设置头文件目录
set_target_properties(SDL2_ttf PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 对于Windows平台，设置导入库位置
if(WIN32)
  set_target_properties(SDL2_ttf PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/SDL2_ttf.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/SDL2_ttf.dll"
  )
else()
  set_target_properties(SDL2_ttf PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libSDL2_ttf.so"
  )
endif()

# 显式地设置SDL2_ttf依赖于SDL2
target_link_libraries(SDL2_ttf INTERFACE SDL2)
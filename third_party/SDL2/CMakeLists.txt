# 设置 CMP0111 策略
cmake_policy(SET CMP0111 NEW)

# 定义SDL2库
add_library(SDL2 SHARED IMPORTED GLOBAL)
add_library(SDL2main STATIC IMPORTED GLOBAL)

# 设置头文件目录
set_target_properties(SDL2 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(SDL2main PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 对于Windows平台，设置导入库位置
if(WIN32)
  # SDL2库的所有配置
  set_target_properties(SDL2 PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/SDL2.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/SDL2.dll"
  )
  
  # SDL2main库的所有配置
  set_target_properties(SDL2main PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/SDL2main.lib"
  )
else()
  # 非Windows平台的设置
  set_target_properties(SDL2 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libSDL2.so"
  )
  set_target_properties(SDL2main PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libSDL2main.a"
  )
endif()
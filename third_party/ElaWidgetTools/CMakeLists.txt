# 设置 CMP0111 策略
cmake_policy(SET CMP0111 NEW)

# 定义ElaWidgetTools库
add_library(ElaWidgetTools SHARED IMPORTED GLOBAL)

# 设置头文件目录
set_target_properties(ElaWidgetTools PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 对于Windows平台，设置导入库位置
if(WIN32)
  set_target_properties(ElaWidgetTools PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/ElaWidgetToolsd.lib"
  )
else()
  set_target_properties(ElaWidgetTools PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/ElaWidgetToolsd.a"
  )
endif()
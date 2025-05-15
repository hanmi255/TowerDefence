# Boost库依赖管理

# 查找boost.asio包
find_package ( Boost REQUIRED COMPONENTS system )

# 创建一个接口库，方便在项目中的其他部分使用
add_library ( boost_lib INTERFACE )

# 设置包含目录和链接库
target_include_directories ( boost_lib INTERFACE ${Boost_INCLUDE_DIRS} )
target_link_libraries ( boost_lib INTERFACE ${Boost_LIBRARIES} )

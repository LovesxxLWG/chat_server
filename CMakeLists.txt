cmake_minimum_required(VERSION 3.22)
project(chat_sever)
#编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}-g)

set(CMAKE_CXX_STANDARD 11)
#可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
#加载子目录
add_subdirectory(src)
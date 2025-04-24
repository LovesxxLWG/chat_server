#定义了SRC_LIST变量，包含该目录所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEl_LIST)
aux_source_directory(./redis REDIS_LIST)
#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
#指定可执行文件需要依赖的库
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)
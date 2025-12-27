add_library(msg_handler STATIC src/msg_handler.c src/msg_definitions.c)
target_include_directories(msg_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(msg_handler PRIVATE common uart uart_stack led flash PROTOBUF protobuf_handler)
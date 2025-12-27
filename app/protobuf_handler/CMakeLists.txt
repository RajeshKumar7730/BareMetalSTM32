add_library(protobuf_handler STATIC src/protobuf_handler.c)
target_include_directories(protobuf_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(protobuf_handler PUBLIC PROTOBUF uart_stack common flash uart led)
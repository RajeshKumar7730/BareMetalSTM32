
add_library(uart_fsm STATIC  src/uart_fsm.c)
target_include_directories(uart_fsm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(uart_fsm PRIVATE common uart led  flash)


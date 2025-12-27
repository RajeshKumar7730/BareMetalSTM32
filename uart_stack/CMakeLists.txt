


add_library(uart_stack STATIC src/isotp.c src/uart_stack.c src/utils.c)
target_include_directories(uart_stack PUBLIC inc)
target_link_libraries(uart_stack PRIVATE uart common)
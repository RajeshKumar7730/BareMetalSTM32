


add_library(FW_UPDATE STATIC src/fw_update.c)
target_include_directories(FW_UPDATE PUBLIC inc/)
target_link_libraries(FW_UPDATE PRIVATE common uart_stack)
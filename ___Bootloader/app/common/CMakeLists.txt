



add_library(common STATIC src/common.c printf-master/printf.c src/rb.c src/crc.c src/sys.c src/queue.c)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc )
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/printf-master)
target_compile_definitions(common PRIVATE PRINTF_DISABLE_SUPPORT_FLOAT PRINTF_DISABLE_SUPPORT_EXPONENTIAL PRINTF_DISABLE_SUPPORT_LONG_LONG )

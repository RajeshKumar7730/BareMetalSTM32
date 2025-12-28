
add_library(flash STATIC src/flash.c)
target_include_directories(flash PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(flash PRIVATE common )
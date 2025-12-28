
add_library(led STATIC src/led.c)
target_include_directories(led PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(led PRIVATE common)
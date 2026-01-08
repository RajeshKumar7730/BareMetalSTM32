


cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE platform/toolchain.cmake)
project(bare-metal-stm32)

# set(CMAKE_C_COMPILER "arm-none-eabi-gcc" CACHE STRING "C Compiler")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/linker_script.ld)
set(MAP_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb -Wall -std=c99  -g -nostdlib")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3 -ggdb -fno-inline -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -ggdb -fno-inline -fno-omit-frame-pointer")


include_directories(
    ${CMAKE_SOURCE_DIR}/__STM32Cube_FW_L4_V1.18.0__/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${CMAKE_SOURCE_DIR}/__STM32Cube_FW_L4_V1.18.0__/Drivers/CMSIS/Core/Include


)

set(SOURCE_FILES
    platform/main.c
    platform/startup.c)

set(EXECUTABLE_NAME ${PROJECT_NAME}.elf)

set(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FPU_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${FPU_FLAGS}")

add_subdirectory(app)
add_subdirectory(drivers)
add_subdirectory(uart_stack)
add_subdirectory(protobuf)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE led common uart  uart_fsm flash uart_stack PROTOBUF uart_stack protobuf_handler DMA FW_UPDATE) # freertos_kernel freertos_config -lc )#spi i2c timers adc
# 
target_link_options(${EXECUTABLE_NAME} PRIVATE "-Wl,-T${LINKER_SCRIPT}")
target_link_options(${EXECUTABLE_NAME} PRIVATE "-Wl,-Map=${MAP_FILE}" -specs=nosys.specs -specs=nano.specs)


if(CMAKE_BUILD_TYPE STREQUAL "debug")
    message("Debug build")
endif()

# add_custom_target(flash
#     COMMAND openocd -f ${CMAKE_SOURCE_DIR}/openocd.cfg -c "program ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
#     DEPENDS ${EXECUTABLE_NAME}
# )


add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
)






set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc" CACHE FILEPATH "C compiler for cross-compilation")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Avoid CMake trying to run the test executable
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


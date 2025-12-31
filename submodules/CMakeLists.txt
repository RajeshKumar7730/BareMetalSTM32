



set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

add_subdirectory(FreeRTOS-Kernel)
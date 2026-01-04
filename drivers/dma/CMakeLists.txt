



add_library(DMA STATIC src/dma.c)
target_include_directories(DMA PUBLIC inc/)
target_link_libraries(DMA PRIVATE common)

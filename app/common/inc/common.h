
#ifndef COMMON_H
#define COMMON_H
#include "stm32l476xx.h"
#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "printf.h"
#include "queue.h"
#define SRAM_START_ADDRESSS     (0x20000000)

#define MAJOR_VERSION_Pos       (16)
#define MINOR_VERSION_Pos       (8)
#define PATCH_VERSION_Pos       (0)
#define MAJOR_VERSION_MASK      (0xFF << MAJOR_VERSION_Pos)
#define MINOR_VERSION_MASK      (0xFF << MINOR_VERSION_Pos)
#define PATCH_VERSION_MASK      (0xFF << PATCH_VERSION_Pos)
#define GET_MAJOR_VERSION(fw_version) ((fw_version & MAJOR_VERSION_MASK ) >> MAJOR_VERSION_Pos)
#define GET_MINOR_VERSION(fw_version) ((fw_version & MINOR_VERSION_MASK ) >> MINOR_VERSION_Pos)
#define GET_PATCH_VERSION(fw_version) ((fw_version & PATCH_VERSION_MASK ) >> PATCH_VERSION_Pos)

typedef struct {
    uint32_t magic_number;
    uint32_t version;   //    Major [31:16]  | Minor [15:8]  | Patch[7:0]
    uint32_t size;      // Size in bytes
    uint32_t crc;       // 8 bit CRC for now. TODO: Make it 32 bit
    uint32_t build_time;  
} fw_meta_t;
void delay();
typedef void(*task_fn)();
void mycopy(uint8_t *dest, uint8_t *src, int n);
uint32_t get_fw_version();
uint32_t get_fw_crc();
#endif

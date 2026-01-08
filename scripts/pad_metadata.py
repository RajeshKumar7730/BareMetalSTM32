import struct
import zlib
import time
import os
import json
FW_BIN = "build/bare-metal-stm32.bin"
FW_OUT = "build/bare-metal-stm32-with-metadata.bin"
MAGIC = 0xDEADBEEF

CRC8_POLYNOMIAL = 0x31
CRC8_INITIAL = 0x00   # or change if your C code uses a different init value

def crc8(data: bytes) -> int:
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ CRC8_POLYNOMIAL) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc


def crc8_file(path):
    with open(path, "rb") as f:
        data = f.read()
    return crc8(data)


FW_VERSION = 0
MAJOR = 0
MINOR = 0
PATCH = 0
with open('version.json') as f:
    d = json.load(f)
    MAJOR = d["major"]
    MINOR = d["minor"]
    PATCH = d["patch"]

FW_VERSION = (MAJOR << 16) | (MINOR << 8) | (PATCH)
# Read firmware
with open(FW_BIN, "rb") as f:
    fw_data = f.read()

fw_size = len(fw_data)
fw_crc = crc8_file(FW_BIN)

# Get build time in seconds since epoch
build_time = int(time.time())  # 32-bit epoch seconds

# Pack metadata: magic, version, size, crc, build_time (total 20 bytes)
meta_data = struct.pack("<IIIII", MAGIC, FW_VERSION, fw_size, fw_crc, build_time)

# Pad metadata to 128-byte alignment
pad_size = (128 - (len(meta_data) % 128)) % 128
meta_data += b'\xFF' * pad_size

# Prepend metadata to firmware
with open(FW_OUT, "wb") as f:
    f.write(meta_data)
    f.write(fw_data)

print(f"Output: {FW_OUT}")
print(f"Metadata size (with padding): {len(meta_data)}, Firmware size: {fw_size}")
# print(f"Firmware will start at 128-byte aligned address after metadata")
print(f"Build epoch time: {hex(build_time)}")
print(f"FW CRC: {hex(fw_crc)}")
print(f"FW Version: {MAJOR,MINOR,PATCH}")
print(f"Packed Version: {hex(FW_VERSION)}")
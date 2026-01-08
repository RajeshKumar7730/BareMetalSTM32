
import sys
import os
import my_msgs_pb2

import struct
import sys
from pathlib import Path
from msgs import *

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "py_gen", "msgs"))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "nanopb", "generator", "proto"))



def file_to_uint32_array(path: str):
    with open(path, "rb") as f:
        data = f.read()

    # Pad to 4-byte boundary (uint32_t)
    if len(data) % 4 != 0:
        data += b"\x00" * (4 - (len(data) % 4))

    count = len(data) // 4

    # Little-endian uint32_t
    return list(struct.unpack("<" + "I" * count, data))



STATE_IDLE = 0
SEND_ERASE_COMMAND = 1
SEND_DATA_CHUNK = 2
WORDS_PER_CHUNK = 8   # =8 * 4 bytes = 32 bytes

def start_fw_upgrade(ser):
    curr_state = 0; 

    fw = file_to_uint32_array("build/bare-metal-stm32-with-metadata.bin")
    while(1):
        match curr_state:
            case 0:
                send_sw_info_req(ser)
                print("Sending sw req")
                response = recv_isotp_message(ser,1)
                print_response(response)
                if response is None:
                    break;
                else:
                    curr_state = SEND_ERASE_COMMAND
                
            case 1:
                print("Sending erase ")
                send_erase_command(ser,40*1024)
                response = recv_isotp_message(ser,1)
                print_response(response)
                if response is None:
                    break;
                else:
                    curr_state = SEND_DATA_CHUNK
            case 2:
                print("Sending data")
                print(len(fw))
                time.sleep(1)
                for i in range(0, len(fw), WORDS_PER_CHUNK):
                    chunk = fw[i:i + WORDS_PER_CHUNK]

                    # Pad last chunk if odd number of words
                    if len(chunk) < WORDS_PER_CHUNK:
                        chunk.append(0)
                    # print([f"0x{v:08X}" for v in chunk])
                    send_data_chunk(ser,chunk)
                    time.sleep(0.001)
                    response = recv_isotp_message(ser, 2)
                    print_response(response)
                    if response is None:
                        break
                    else:
                        continue
                
                print("Notify Update Done.")
                notify_fw_upd_complete(ser)
                response = recv_isotp_message(ser,1)
                print_response(response)
                if response is None:
                    break;
                else:
                    # Check crc from response 
                    pass                  
                print("Send reset ")
                send_reset_command(ser)
                response = recv_isotp_message(ser,1)
                print_response(response)
                if response is None:
                    break;
                else:
                    pass

                print("FW Update successfully done")
                break           
                    
                
                  









import serial
import time
import math
import struct
import random
import sys
import platform
import os
# sys.path.append("D:\Embedded Workspace\BareMetalSTM32\protobuf\py_gen\msgs")  # relative path to your generated code
# sys.path.append("D:/Embedded Workspace/BareMetalSTM32/protobuf/nanopb/generator/proto")



BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "py_gen", "msgs"))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "nanopb", "generator", "proto"))

import my_msgs_pb2
from uart_transfer import *
from fw_upd import *
from msgs import *
def form_int(str):
    rev=""
    for i in range(0,len(str),2):
        rev=str[i:i+2]+rev
        #print(rev)
    return rev



# ---------------- MAIN ----------------
def main():
    ser = serial.Serial("COM6", 115200, timeout=1)
    try:
        while True:
            msg_id = int(input("\nmsg_id (1 - Add 2 - sw info req 3- fw upd ): "))

            request = my_msgs_pb2.proto_msg()
            if msg_id == 1:
                a = int(input("Enter first num : "))
                b = int(input("Enter second num: "))
                send_add_req(ser,a,b)
            elif msg_id == 2:
                send_sw_info_req(ser)
            elif msg_id == 3:
                start_fw_upgrade(ser)
                continue;
            else:
                print("Invalid msg_id")
                continue

            print("Waiting for response...")
            rx = recv_isotp_message(ser,2)
            if rx is None:
                print("Timeout")
                continue
            # print("RAW RX:", rx.hex(" "))
            msg = my_msgs_pb2.proto_msg()
            msg.ParseFromString(rx)
            print(msg)

    finally:
        ser.close()

if __name__ == "__main__":
    main()
 
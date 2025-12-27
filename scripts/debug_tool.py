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
SOF = 0x7E
TYPE_REQUEST = 0x00
TYPE_RESPONSE = 0x01
CRC8_POLYNOMIAL = 0x31


def form_int(str):
    rev=""
    for i in range(0,len(str),2):
        rev=str[i:i+2]+rev
        #print(rev)
    return rev
# ---------------- CRC ----------------
def calculate_crc8(data: bytes) -> int:
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ CRC8_POLYNOMIAL) & 0xFF if (crc & 0x80) else (crc << 1) & 0xFF
    return crc

# ---------------- UART PACKET ----------------
def send_uart_packet(ser, payload: bytes):
    frame = bytes([
        SOF,
        0xDE,
        calculate_crc8(payload),
        len(payload)
    ]) + payload
    ser.write(frame)

# ---------------- ISO-TP SEND ----------------
def send_isotp_message(ser, data: bytes):
    length = len(data)

    if length <= 15:
        payload = bytes([1, length]) + data
        send_uart_packet(ser, payload)
        return

    num_chunks = math.ceil(length / 15)
    for seq in range(num_chunks):
        chunk = data[seq * 15:(seq + 1) * 15]
        payload = bytes([2, length]) + chunk if seq == 0 else bytes([3, seq]) + chunk
        send_uart_packet(ser, payload)
        time.sleep(0.01)

# ---------------- ISO-TP RECEIVE ----------------
def recv_isotp_message(ser) -> bytes | None:
    data_buffer = bytearray()
    expected_len = None

    while True:
        header = ser.read(4)
        if len(header) < 4:
            return None

        if header[0] != SOF:
            continue

        payload_len = header[3]
        payload = ser.read(payload_len)
        if len(payload) != payload_len:
            continue

        pci = payload[0]

        # Single Frame
        if pci == 1:
            return payload[2:2 + payload[1]]

        # First Frame
        if pci == 2:
            expected_len = payload[1]
            data_buffer.extend(payload[2:])
            continue

        # Consecutive Frame
        if pci == 3:
            data_buffer.extend(payload[2:])
            if expected_len is not None and len(data_buffer) >= expected_len:
                return data_buffer[:expected_len]

# ---------------- MSG DECODE ----------------
def decode_msg(msg: bytes):
    msg_id = msg[0]
    flags = msg[1]
    msg_len = msg[2]
    data = msg[3:3 + msg_len]
    return msg_id, flags, data

# ---------------- SW INFO DECODE ----------------
def decode_sw_info(data: bytes):
    banks = []
    for i in range(0, len(data), 5):
        banks.append({
            "active": bool(data[i]),
            "major": data[i+1],
            "minor": data[i+2],
            "version": data[i+3],
            "hash": data[i+4]
        })
    return banks

# ---------------- STATS DECODE (WITH PADDING) ----------------
def decode_stats(data: bytes):
    """
    C layout (with padding):
    uint32_t prevState
    uint32_t currState
    char errors[5]
    char padding[3]
    float num
    TOTAL = 20 bytes
    """

    if len(data) < 20:
        raise ValueError("Invalid stats payload length")

    prevState, currState = struct.unpack_from("<II", data, 0)

    errors_raw = data[8:13]           # 5 bytes
    errors = errors_raw.split(b'\x00', 1)[0].decode('ascii', errors='ignore')

    num = struct.unpack_from("<f", data, 16)[0]  # float after padding

    return {
        "prevState": prevState,
        "currState": currState,
        "errors": errors,
        "num": num
    }


# ---------------- MAIN ----------------
def main():
    os_name = platform.system()
    if os_name=="Windows":
        ser = serial.Serial("COM6", 115200, timeout=1)
    elif os_name=="Darwin":
        ser = serial.Serial("COM6", 115200, timeout=1)
    print("UART ready")

    try:
        while True:
            msg_id = int(input("\nmsg_id (1 - Add 2 - sw info req 3- fw upd ): "))

            # -------- BUILD REQUEST --------
            

            if msg_id == 1:
                a = int(input("A: "))
                b = int(input("B: "))
                request = my_msgs_pb2.proto_msg()
                request.add_req.a = a
                request.add_req.b = b
                data = request.SerializeToString()
                print("Serialized bytes:", data)
                msg = bytes(data)
            
            elif msg_id == 2:
                request = my_msgs_pb2.proto_msg()
                request.sw_info_req.SetInParent()
                data = request.SerializeToString()
                msg = bytes(data)

            elif msg_id == 3:


                with open("../build/bare-metal-stm32-with-metadata.bin", "rb") as file:
                    data = file.read()
                    fw_string=data.hex()
                    # print(len(fw_string))
                    chunks = []

                    request = my_msgs_pb2.proto_msg()
                    request.upd_req.cmd = 0
                    data = request.SerializeToString()
                    msg=bytes(data)
                    send_isotp_message(ser, msg)
                    time.sleep(0.5)


                    for i in range(0, len(fw_string), 8):
                        #Send data as uint32_t
                        #So get 32 bits in little endian and form the int from bin
                        # When we have 16 bytes, send a full frame
                        temp_str = fw_string[i:i+8]
                        chunks.append(int(form_int(temp_str),16))
                        if len(chunks) == 8:
                            #print(chunks)
                            print('\t'.join([hex(i) for i in chunks]))
                            request = my_msgs_pb2.proto_msg()
                            request.upd_req.cmd = 1
                            request.upd_req.data.extend(chunks)
                            data = request.SerializeToString()
                            msg=bytes(data)
                            send_isotp_message(ser, msg)
                            time.sleep(0.0025)
                            chunks = []

                    if chunks:
                        while len(chunks) < 8:
                            chunks.append(0xFF)
                        request = my_msgs_pb2.proto_msg()
                        request.upd_req.cmd = 1
                        request.upd_req.data.extend(chunks)
                        data = request.SerializeToString()
                        msg=bytes(data)
                        send_isotp_message(ser, msg)
                    
                    time.sleep(0.05)
                    request = my_msgs_pb2.proto_msg()
                    request.upd_req.cmd = 3
                    chunks=[]
                    request.upd_req.data.extend(chunks)
                    data = request.SerializeToString()
                    msg=bytes(data)
                    send_isotp_message(ser, msg)
                    print("FW Upd Finished. Reset Command Sent")
            else:
                print("Invalid msg_id")
                continue

            send_isotp_message(ser, msg)
            print("Waiting for response...")

            rx = recv_isotp_message(ser)
            if rx is None:
                print("Timeout")
                continue

            print("RAW RX:", rx.hex(" "))

            msg = my_msgs_pb2.proto_msg()
            msg.ParseFromString(rx)

            print(msg)

    except KeyboardInterrupt:
        print("\nExit")
    finally:
        ser.close()

if __name__ == "__main__":
    main()
 
import os
import sys
from pathlib import Path

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "py_gen", "msgs"))
sys.path.append(os.path.join(BASE_DIR, "..", "protobuf", "nanopb", "generator", "proto"))
from uart_transfer import *
import my_msgs_pb2

def send_add_req(ser,a,b):
    request = my_msgs_pb2.proto_msg()
    request.add_req.a = a
    request.add_req.b = b
    send_msg(ser, request)

def send_sw_info_req(ser):
    request = my_msgs_pb2.proto_msg()
    request.sw_info_req.SetInParent()
    send_msg(ser, request)

def print_response(response):
    if response is None:
        print("Timeout for response")
        return
    msg = my_msgs_pb2.proto_msg()
    msg.ParseFromString(response)
    print(msg)

def send_erase_command(ser,fw_size):
    request = my_msgs_pb2.proto_msg()
    request.upd_req.cmd  = 0
    request.upd_req.fw_size = fw_size
    send_msg(ser,request)

def send_data_chunk(ser,chunk):
    request = my_msgs_pb2.proto_msg()
    request.upd_req.cmd = 1   # DATA_CHUNK
    request.upd_req.data.extend(chunk)
    send_msg(ser,request)

def notify_fw_upd_complete(ser):
    request = my_msgs_pb2.proto_msg()
    request.upd_req.cmd = 3
    send_msg(ser,request)

def send_reset_command(ser):
    request = my_msgs_pb2.proto_msg()
    request.upd_req.cmd = 4
    send_msg(ser,request)





import serial
import struct
import time

SOF = 0x7E
DEBUG = False
HEADER_FMT = "<I"  # 32-bit header (matches C union)
HEADER_SIZE = struct.calcsize(HEADER_FMT)



CRC8_POLYNOMIAL = 0x31  # change to match your C code

def calculate_crc8(data: bytes) -> int:
    crc = 0x00  # same initial value as your C code

    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:   # MSB set?
                crc = ((crc << 1) ^ CRC8_POLYNOMIAL) & 0xFF
            else:
                crc = (crc << 1) & 0xFF

    return crc

def debug_log(*args, **kwargs):
    if DEBUG:
        print(*args, **kwargs)


def make_header(msg_id, msg_flags, data_length, crc):
    header_int = (crc << 24) | (data_length << 16) | (msg_flags << 8) | msg_id
    return struct.pack(HEADER_FMT, header_int)


def send_msg(ser, msg_id, data=b""):
    crc = calculate_crc8(data)
    header = make_header(msg_id, 0, len(data), crc)
    frame = bytes([SOF]) + header + data
    for b in frame:
        ser.write(bytes([b]))
        # time.sleep(0.005)  # adjust as needed

    debug_log(f"📤 Sent: {frame.hex(' ')}")



def read_exact(ser, length, timeout=2.0):
    data = b""
    start = time.time()
    while len(data) < length and (time.time() - start) < timeout:
        chunk = ser.read(length - len(data))
        if chunk:
            data += chunk
    return data if len(data) == length else None


def receive_msg(ser, timeout=2.0):
    start_time = time.time()
    while time.time() - start_time < timeout:
        sof = read_exact(ser, 1, timeout)
        if not sof or sof[0] != SOF:
            continue

        header_bytes = read_exact(ser, HEADER_SIZE, timeout)
        if not header_bytes:
            print("❌ Incomplete header")
            return None

        header_int, = struct.unpack(HEADER_FMT, header_bytes)
        msg_id = header_int & 0xFF
        msg_flags = (header_int >> 8) & 0xFF
        data_length = (header_int >> 16) & 0xFF
        protocol_version = (header_int >> 24) & 0xFF

        data = read_exact(ser, data_length, timeout)
        if data is None:
            print("❌ Incomplete data")
            return None

        debug_log(f"Received SOF, Header: {header_bytes.hex(' ')}, Data: {data.hex(' ')}")
        return {
            "msg_id": msg_id,
            "msg_flags": msg_flags,
            "protocol_version": protocol_version,
            "data_length": data_length,
            "data": data
        }

    print("⏱ Timeout waiting for message")
    return None


# ------------------ Handlers ------------------

def handle_default(data):
    print("DEFAULT STRING:", data.decode("ascii", errors="replace"))


def handle_sw_info(data):
    if len(data) != 10:
        print("⚠️ Unexpected SW_INFO length:", len(data))
        return
    bank1 = struct.unpack("<?BBBB", data[:5])
    bank2 = struct.unpack("<?BBBB", data[5:10])
    print("SW INFO:")
    print(f"  Bank1: active={bank1[0]}, version={bank1[1]}.{bank1[2]}.{bank1[3]}, hash={bank1[4]}")
    print(f"  Bank2: active={bank2[0]}, version={bank2[1]}.{bank2[2]}.{bank2[3]}, hash={bank2[4]}")


def handle_stats(data):
    if len(data) != 20:
        print("⚠️ Unexpected STATS length:", len(data))
        return
    prevState, currState, errors_bytes, num = struct.unpack("<II5s3xf", data)
    errors = errors_bytes.decode("ascii", errors="replace").rstrip('\x00')
    print("STATS:")
    print(f"  prevState={prevState}, currState={currState}, errors={errors}, num={num}")


def handle_addition(data):
    if len(data) != 1:
        print("⚠️ Unexpected addition length:", len(data))
        return
    print("SUM RESULT:", data[0])


def handle_owner_name(data):
    print("OWNER NAME:", data.decode("ascii", errors="replace"))


def handle_toggle_led(data):
    if len(data) != 2:
        print("⚠️ Unexpected toggle LED length:", len(data))
        return
    prevState, currState = struct.unpack("<BB", data)
    print(f"LED toggled: {'ON' if prevState else 'OFF'} → {'ON' if currState else 'OFF'}")


# ------------------ Message Registry ------------------

MSG_REGISTRY = {
    0: {"name": "DEFAULT",      "handler": handle_default,      "prompt": None},
    1: {"name": "SW_INFO",      "handler": handle_sw_info,      "prompt": None},
    2: {"name": "STATS",        "handler": handle_stats,        "prompt": None},
    3: {"name": "ADDITION",     "handler": handle_addition,     "prompt": "two uint8 numbers"},
    4: {"name": "OWNER_NAME",   "handler": handle_owner_name,   "prompt": None},
    5: {"name": "TOGGLE_LED",   "handler": handle_toggle_led,   "prompt": None},
    6: {"name": "SW Reset",     "handler": handle_default,      "prompt": None},
    7: {"name": "Start FW Upd", "handler": handle_default,      "prompt": None},
}


def decode_msg(msg):
    print("\n✅ Received message:")
    print(f" msg_id: {msg['msg_id']}, protocol_version: {msg['protocol_version']}, data_length: {msg['data_length']}")
    entry = MSG_REGISTRY.get(msg['msg_id'])
    if entry:
        entry["handler"](msg["data"])
    else:
        print("⚠️ Unknown msg_id")


# ------------------ Main ------------------

def main():
    global DEBUG
    DEBUG = input("Enable debug logs? (y/n): ").strip().lower() == "y"

    try:
        ser = serial.Serial(port='COM6', baudrate=115200, timeout=1)
    except serial.SerialException as e:
        print("Serial error:", e)
        return

    print("Press Ctrl+C to exit.")
    try:
        while True:
            print("\nAvailable messages:")
            for mid, info in MSG_REGISTRY.items():
                print(f" {mid} = {info['name']}")
            user_input = input("Enter msg_id (q=quit): ").strip()
            if user_input.lower() == 'q':
                break

            try:
                msg_id = int(user_input)
                entry = MSG_REGISTRY.get(msg_id)
                if not entry:
                    print("❌ Unsupported msg_id")
                    continue

                data = b""
                if msg_id == 3:  # Addition prompt
                    a = int(input("Enter first number (0–255): "))
                    b = int(input("Enter second number (0–255): "))
                    if not (0 <= a <= 255 and 0 <= b <= 255):
                        print("❌ Numbers must be 0–255")
                        continue
                    data = bytes([a, b])
                if msg_id == 7:
                    
                    data = bytes([1])    # Erase cmd
                    send_msg(ser, msg_id, data)

                    input()     #Wait for terminal input


                    with open("../build/bare-metal-stm32-with-metadata.bin", "rb") as file:
                        data = file.read()
                    fw_string=data.hex()
                    # print(len(fw_string))
                    chunks = []

                    for i in range(0, len(fw_string), 2):
                        chunks.append(int(fw_string[i:i+2], 16))

                        # When we have 16 bytes, send a full frame
                        if len(chunks) == 16:
                            chunks.insert(0,2)    # Write to flash cmd
                            data = bytes(chunks)
                            send_msg(ser, msg_id, data)
                            time.sleep(0.01)
                            chunks = []

                    # Handle the last partial frame
                    if chunks:
                        while len(chunks) < 16:
                            chunks.append(0xFF)
                        chunks.insert(0,2)    # Write to flash cmd
                        data = bytes(chunks)
                        send_msg(ser, msg_id, data)
                    
                    time.sleep(1)
                    data = bytes([3]) # Self reset cmd
                    send_msg(ser, msg_id, data)

                       
                else:
                    send_msg(ser, msg_id, data)
                    response = receive_msg(ser)
                    if response:
                        decode_msg(response)

            except ValueError:
                print("❌ Invalid input")

    except KeyboardInterrupt:
        print("\nExiting.")
    finally:
        ser.close()


if __name__ == "__main__":
    main()

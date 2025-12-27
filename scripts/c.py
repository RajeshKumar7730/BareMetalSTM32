import serial
import time
import math
import random

SOF = 0x7E
DEBUG = False
CRC8_POLYNOMIAL = 0x31  # change to match your C code

# ----------------- Utilities -----------------

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


def debug_log(*args):
    if DEBUG:
        print(*args)

def send_msg(ser, payload: bytes):
    """
    UART packet format:
    [SOF][0x12][0x34][payload_len][payload...]
    """
    frame = bytes([
        SOF,
        0xDE,
        calculate_crc8(payload),
        len(payload)
    ]) + payload

    ser.write(frame)   # write ONCE, not byte-by-byte
    debug_log(f"ISOTP Frame: {payload.hex(' ')}")
    debug_log(f"UART Packet  : {frame.hex(' ')}")

# ----------------- ISO-TP Framing -----------------

def send_isotp_message(ser, data: bytes):
    length = len(data)

    if length <= 15:
        # Single Frame
        payload = bytes([1, length]) + data
        send_msg(ser, payload)
        return

    # Multi Frame
    num_chunks = math.ceil(length / 15)
    print(f"Total Chunks: {num_chunks}")

    for seq in range(num_chunks):
        chunk = data[seq * 15:(seq + 1) * 15]

        if seq == 0:
            # First Frame
            payload = bytes([2, length]) + chunk
        else:
            # Consecutive Frame
            payload = bytes([3, seq]) + chunk

        print(f"Sending chunk {seq + 1}")
        send_msg(ser, payload)
        time.sleep(0.01)

    print("MF sent")

# ----------------- Main -----------------

def main():
    global DEBUG
    DEBUG = input("Enable debug logs? (y/n): ").strip().lower() == "y"

    try:
        ser = serial.Serial(port="COM6", baudrate=115200, timeout=1)
    except serial.SerialException as e:
        print("Serial error:", e)
        return

    print("Press Ctrl+C to exit.")

    try:
        while True:
            try:
                length = int(input("\nNo of bytes to send (0–255): "))
                if not 0 <= length <= 255:
                    print("Invalid length")
                    continue

                data = bytes(random.randint(1, 10) for _ in range(length))
                debug_log(f"Data: {data.hex(' ')}")

                send_isotp_message(ser, data)

            except ValueError:
                print("Invalid input")

    except KeyboardInterrupt:
        print("\nExiting.")
    finally:
        ser.close()

if __name__ == "__main__":
    main()

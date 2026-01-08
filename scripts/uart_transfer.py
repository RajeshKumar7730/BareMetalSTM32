
import math
import time
SOF = 0x7E

CRC8_POLYNOMIAL = 0x31

def calculate_crc8(data: bytes) -> int:
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ CRC8_POLYNOMIAL) & 0xFF if (crc & 0x80) else (crc << 1) & 0xFF
    return crc



def send_msg(ser,req):
    msg = req.SerializeToString();
    send_isotp_message(ser,msg)


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

def recv_isotp_message(ser, timeout) -> bytes | None:
    data_buffer = bytearray()
    expected_len = None

    start_time = time.monotonic()

    while True:
        # Timeout check
        if time.monotonic() - start_time > timeout:
            return None

        header = ser.read(4)
        if len(header) < 4:
            continue

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
            data_buffer.clear()
            data_buffer.extend(payload[2:])
            continue

        # Consecutive Frame
        if pci == 3:
            data_buffer.extend(payload[2:])
            if expected_len is not None and len(data_buffer) >= expected_len:
                return data_buffer[:expected_len]

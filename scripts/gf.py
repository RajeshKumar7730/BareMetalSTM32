import serial

PORT = "COM6"          # change as needed
BAUDRATE = 115200

ser = serial.Serial(PORT, BAUDRATE, timeout=1)

STATE_WAIT_SOF = 0
STATE_READ_HDR = 1
STATE_READ_PAYLOAD = 2

state = STATE_WAIT_SOF
packet = bytearray()
payload_len = 0

print("Listening for ISO-TP frames...")

while True:
    b = ser.read(1)
    if not b:
        continue

    byte = b[0]

    # ---- UART packet state machine ----
    if state == STATE_WAIT_SOF:
        if byte == 0x7E:
            packet.clear()
            packet.append(byte)
            state = STATE_READ_HDR

    elif state == STATE_READ_HDR:
        packet.append(byte)
        if len(packet) == 4:   # SOF + hdr + crc + payload_len
            payload_len = packet[3]
            state = STATE_READ_PAYLOAD

    elif state == STATE_READ_PAYLOAD:
        packet.append(byte)
        if len(packet) == 4 + payload_len:
            payload = packet[4:]

            # ---- ISO-TP decode ----
            if len(payload) < 2:
                state = STATE_WAIT_SOF
                continue

            pci = payload[0]
            info = payload[1]
            data = payload[2:]

            data_hex = ", ".join(f"0x{b:02X}" for b in data)

            if pci == 1:
                print("ISO-TP SF")
                print(f"  Length : {info}")
                print(f"  Data   : [{data_hex}]")

            elif pci == 2:
                print("ISO-TP FF")
                print(f"  Total Length : {info}")
                print(f"  Data         : [{data_hex}]")

            elif pci == 3:
                print("ISO-TP CF")
                print(f"  Seq Num : {info}")
                print(f"  Data    : [{data_hex}]")

            else:
                print(f"Unknown PCI : {pci}")

            print("-" * 40)
            state = STATE_WAIT_SOF

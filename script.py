import serial

port = 'COM6'
baud_rate = 19200

serial_port = serial.Serial(port, baud_rate, timeout=1)

while True:
    data = serial_port.readline().decode().strip()
    if data:
        print(data)

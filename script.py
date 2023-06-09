import serial

# Define the serial port and baud rate
port = 'COM5'  # Replace 'COMX' with the assigned COM port of your Arduino Bluetooth module
baud_rate = 9600

# Create a serial object
serial_port = serial.Serial(port, baud_rate, timeout=1)

# Read the incoming data
while True:
    data = serial_port.readline().decode().strip()
    if data:
        print(data)

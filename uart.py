import serial
import sys

def main(port, baudrate):
    ser = serial.Serial(port, baudrate)
    while True:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting).decode('utf-8')
            print(data, end='')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python uart.py <port> <baudrate>")
        sys.exit(1)
    
    port = sys.argv[1]
    baudrate = int(sys.argv[2])
    main(port, baudrate)
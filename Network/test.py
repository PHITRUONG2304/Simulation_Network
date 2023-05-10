import serial 
import time

mSerial = serial.Serial("COM12", 9600)

while True:
    try:
        if mSerial.in_waiting:
            start = time.time_ns()
            data = mSerial.read(mSerial.in_waiting)
            print(data.hex())
            end = time.time_ns()
    except KeyboardInterrupt:
        break
        
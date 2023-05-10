import serial 
import time

mSerial = serial.Serial("COM13", 115200, timeout= 0)
while True:
    try:
        if mSerial.in_waiting:
            start = time.time_ns()
            data = mSerial.read(mSerial.in_waiting)
            print(data.hex())
            end = time.time_ns()
            print(end - start)
    except KeyboardInterrupt:
        break
        
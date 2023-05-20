import time
import serial
import logging
import threading

# def formatBytes(raw):
#     try:
#         result = 0
#         for i in range(len(raw)):
#             result |= raw[i] << (8*i);
#         return result
#     except:
#         return raw


# mSerial = serial.Serial("COM4",baudrate=9600)
# logging.basicConfig(filename="log.txt", filemode="w", level=logging.DEBUG, format=None)

def printHello():
    while True:
        print("Hello world")
        time.sleep(5)
        
def printHello1():
    while True:
        print("Hello bro")
        time.sleep(2)

def main():
    threading.Thread(target=printHello, daemon=True).start()
    threading.Thread(target=printHello1, daemon=True).start()
    while True:
        try:
            
            pass
        except KeyboardInterrupt:
            break

if __name__ == "__main__":
    main()
import time
import serial
import logging
import threading

HEADER_SIZE = 4*2 + 4 + 1

def isValidMessage(msg):
    if(len(msg) <= HEADER_SIZE): return False
    sum = 0
    for i in range(len(msg)):
        sum += msg[i]
    if(sum%256 != 0):
        return False
    return True

def main():
    msg = bytes([0x0b,0x00,0x17,0x00,0xff,0xff,0x17,0x00,0x10,0x00,0x00,0x00,0x92,0x22,0x05,0x00])
    print(isValidMessage(msg))

if __name__ == "__main__":
    main()
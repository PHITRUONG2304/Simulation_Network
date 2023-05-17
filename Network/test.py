import time
import globalData as globalVar
import serial

# b_array = bytearray([0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07])
mSerial = serial.Serial(port="COM7",baudrate=9600)

def main():
    while True:
        try:
            # if mSerial.in_waiting:
            #     globalVar.logging.debug("Receive: " + str(mSerial.read_all()))
            # for _byte in b_array:
                # mSerial.write(_byte)
                # mSerial.write(bytes(_byte))
                # print(_byte)
                pass
        except KeyboardInterrupt:
            break

if __name__ == "__main__":
    main()
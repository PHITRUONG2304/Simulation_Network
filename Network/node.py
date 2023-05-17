import threading
from serial import Serial
import configurations as conf
import globalData as globalVar
import math
import time

MAXRANGE = 2500
TIME_ON_AIR_SIMULATION = 30

class Node:
    def __init__(self, cordinates, address, channel, serialID, buff):
        self.cordinates = cordinates
        self.buffer = buff
        self.address = address
        self.channel = channel
        self.mSerial = Serial(serialID, baudrate=conf.COMM_BAUDRATE, bytesize=8, parity="N", stopbits=1)
        # Create 2 thread to handle Serial event
        self.receiver = threading.Thread(target=self.run)
        self.transmitter = threading.Thread(target=self.readRxSerial)
        # Start 2 thread
        self.receiver.start()
        self.transmitter.start()

    def isSelf(self, node):
        return self.address == node.address

    def canCommunicate(self, node, channel):
        if self.isSelf(node):
            return False
        if self.channel != channel:
            return False

        distance = math.sqrt(
            math.pow(abs(node.cordinates[0] - self.cordinates[0]), 2)
            + math.pow(abs(node.cordinates[1] - self.cordinates[1]), 2)
        )
        return distance < MAXRANGE

    def pushDataIntoQueue(self, dataArray):
        while not self.buffer.canAccess():
            pass
        self.buffer.writeBuffer(dataArray)
        pass

    def readRxSerial(self):
        while not self.mSerial.is_open:
            pass
        self.mSerial.read_all()
        while self.mSerial.is_open:
            if self.mSerial.in_waiting:
                data = self.mSerial.read_all()
               
                while not globalVar.sharedBuffer.canAccess():
                    pass
                # Write data array into buffer
                globalVar.sharedBuffer.writeBuffer((self.address, data))
            if globalVar.eventBreak.is_set():
                break
            time.sleep(0.001)
        pass

    def run(self):
        while True:
            if self.buffer.available():
                while not self.buffer.canAccess():
                    pass
                data = self.buffer.getBuffer()
                # print("dest:", self.address, ", payload:", data.hex())
                self.mSerial.write(data)
            if globalVar.eventBreak.is_set():
                break
            time.sleep(0.001)
        pass

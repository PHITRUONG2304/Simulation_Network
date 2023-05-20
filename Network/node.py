import threading
from serial import Serial
from buffer import Buffer
from statics import StatisticalModule
import configurations as conf
import globalData as globalVar
import math
import time
import os

MAXRANGE = 2500
TIME_ON_AIR_SIMULATION = 30



class Node:
    def __init__(self, cordinates, address, channel, serialID):
        self.cordinates = cordinates
        self.buffer = Buffer()
        self.address = address
        self.channel = channel
        self.mSerial = Serial(serialID, baudrate=conf.COMM_BAUDRATE, bytesize=8, parity="N", stopbits=1)
        
        #For statics
        if not os.path.exists("log_"+str(address)+".txt"):
            open("log_0x0"+str(hex(address)).replace("x", "")+".txt", 'w').close()
        self.staticsModule = StatisticalModule("log_0x0"+str(hex(address)).replace("x", "")+".txt", address, channel)
        
        # Create 2 thread to handle Serial event
        self.receiver = threading.Thread(target=self.run)
        self.transmitter = threading.Thread(target=self.readRxSerial)
        # Start 2 thread
        self.receiver.start()
        self.transmitter.start()
        
    def printInfo(self):
        self.staticsModule.printInfo()
        pass

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
                self.staticsModule.updateStatisticalLoRaData(data[3:], True) #Update when send
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
                self.mSerial.write(data)
                # For statics
                self.staticsModule.updateStatisticalLoRaData(data, False) #Update when receive
            if globalVar.eventBreak.is_set():
                break
            time.sleep(0.001)
        pass

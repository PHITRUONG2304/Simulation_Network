import threading
import globalData as globalVar
import time
from tabulate import tabulate

SEND = True
RECEIVE = False

def whattime_minute(_start):
    elapsedTime = time.time() - _start
    return elapsedTime/60.0

def formatBytes(raw):
    try:
        result = 0
        for i in range(len(raw)):
            result |= raw[i] << (8*i);
        return result
    except:
        return raw 

def formatAdress(raw):
    return (formatBytes(raw=raw[:2]), raw[2], raw[3])

class StatisticalModule:
    def __init__(self, nameLogFile, id, channel, network = 0):
        self.address = (id, channel, network)
        self.numMyPacket = 0
        self.numReceived = 0
        self.previousTime_1 = time.time()
        self.numSent = 0
        self.previousTime_2 = time.time()
        self.neighbors = []
        self.logFile = globalVar.setup_logger(name=nameLogFile.replace(".txt", ""), fileName=nameLogFile, format=None)
        
        printInfoInterval = threading.Thread(target=self.printInfo)
        printInfoInterval.start()
        pass
    
    def printInfo(self):
        while True:
            self.logFile.info("My address (id, chann, network) is: " + str(self.address))
            self.logFile.info("The number of my packet successfully sent is: " + str(self.numMyPacket))
            self.logFile.info("The incomming rate is (packets/minute): " + str(self.numReceived / whattime_minute(self.previousTime_1)))
            self.logFile.info("The outgoing rate is (packets/minute): " + str(self.numSent/whattime_minute(self.previousTime_2)))
            if len(self.neighbors) > 0:
                header = self.neighbors[0].keys()
                
                self.logFile.info(tabulate([elm.values() for elm in self.neighbors], headers=header, tablefmt="outline", floatfmt=".4f"))
            
            self.logFile.info("") # break line
            if globalVar.eventBreak.is_set():
                break
            time.sleep(10)
    
    def updateStatisticalLoRaData(self, msg, state = SEND):
        if state == SEND:
            updateinfo = threading.Thread(target=self.updateWhenSend, args=(msg, ))
        else:
            updateinfo = threading.Thread(target=self.updateWhenReceive, args=(msg, ))
        
        updateinfo.start()
        pass
    
    def isSelf(self, address):
        if self.address == address:
            return True
        return False
    
    def updateWhenReceive(self, msg):
        # Message content: | Source    | Destination | MsgSize | SumByte | Payload
        #                  | E32_Addr_t| E32_Addr_t  | size_t  | uint8_t | uint8_t[]
        
        source = formatAdress(msg[:4])
        dest = formatAdress(msg[4:8])
        # msgSize = payload[8:12]
        # checksum = payload[12:13]
        payload = msg[13:]
              
        msgType = payload[0]
        if msgType == 0x27: # DATA_PACKET
            #	Payload:|  Type  | Address | hopNum | latency | timeGenerated |
            #			| 1 byte | 4 bytes | 1 byte | 4 bytes |    4 bytes    |
            originalNode = formatAdress(payload[1:5])
            hopNum = formatBytes(payload[5])
            latency = formatBytes(payload[6:9])
            
            if not self.isAlreadyExistNeighbor(originalNode):
                self.addNewNeighbor(originalNode)
            
            for neighbor in self.neighbors:
                if neighbor["address"] == originalNode:
                    neighbor["numReceived"] += 1
                    if neighbor["maxhops"] < hopNum:
                        for i in range(hopNum - neighbor["maxhops"]):
                            neighbor["numReceivedAccordingToHops"].append(0)
                        neighbor["maxhops"] = hopNum
                    neighbor["numReceivedAccordingToHops"][hopNum-1] += 1
                    neighbor["avrLatency"]  = (neighbor["avrLatency"] * (neighbor["numReceived"] - 1)  + latency) / neighbor["numReceived"]
            
            self.numReceived += 1
            pass
        elif msgType == 0x29: # RECEIVED_SUCCESS
            if not self.isAlreadyExistNeighbor(source):
                self.addNewNeighbor(source)
                
            for neighbor in self.neighbors:
                if neighbor["address"] == source:
                    neighbor["numSentSucess"] += 1
                    
        else:
            pass
    
    def updateWhenSend(self, msg):
        # Message content: | Source    | Destination | MsgSize | SumByte | Payload
        #                  | E32_Addr_t| E32_Addr_t  | size_t  | uint8_t | uint8_t[]
        source = formatAdress(msg[:4])
        dest = formatAdress(msg[4:8])
        # msgSize = payload[8:12]
        # checksum = payload[12:13]
        payload = msg[13:]
              
        msgType = payload[0]
        if msgType == 0x27: # DATA_PACKET
            self.numSent += 1
            originalNode = formatAdress(payload[1:5])
            if self.isSelf(originalNode):
                self.numMyPacket += 1
                
            if not self.isAlreadyExistNeighbor(dest):
                self.addNewNeighbor(dest)
                
            for neighbor in self.neighbors:
                if neighbor["address"] == dest:
                    neighbor["numSent"] += 1
            pass
        else:
            pass
    
    def isAlreadyExistNeighbor(self, address):
        if len(self.neighbors) == 0: return False
        else:
            for neighbor in self.neighbors:
                if neighbor["address"] == address:
                    return True
        return False
    
    def addNewNeighbor(self, address):
        if not self.isAlreadyExistNeighbor(address):
            self.neighbors.append({"address":address, "numReceived": 0, "numSentSucess": 0, "numSent":0, "avrLatency": 0, "maxhops": 0, "numReceivedAccordingToHops": []})
            
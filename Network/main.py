from node import Node
from threading import Thread
import configurations as conf
import globalData as glovalVar
import time

# from time import sleep


def createNodes(cordinates, address, channel, serialID):
    nodesList = [
        Node(
            cordinates[i],
            address[i],
            channel[i],
            serialID[i]
        )
        for i in range(conf.NUM_NODES)
    ]
    return nodesList


def getNode(address, nodes):
    for node in nodes:
        if node.address == address:
            return node
    raise NameError


def isBroadcastAddress(dest):
    return dest == 0xFFFF


def descapsulatePacket(packet):
    dest = packet[0] | (packet[1] << 8)
    channel = packet[2]
    payload = packet[3:]
    return (dest, channel, payload)


def main():
    nodes = createNodes(conf.CORDINATES, conf.ADDRESS, conf.CHANNEL, conf.SERIAL_ID)
    # TODO
    # Create a function to transfer data to buffer
    while True:
        try:
            if glovalVar.sharedBuffer.available():
                while not glovalVar.sharedBuffer.canAccess():
                    pass
                try:
                    (source, packet) = glovalVar.sharedBuffer.getBuffer()
                    nodeSource = getNode(source, nodes)
                    (dest, chan, msg) = descapsulatePacket(packet)
                except:
                    continue
                
                glovalVar.log_trace_communicate.debug(str(source) + " --> " + str(dest) + " payload: " + str(msg.hex()))
                
                if isBroadcastAddress(dest):
                    for node in nodes:
                        if nodeSource.canCommunicate(node, chan):
                            node.pushDataIntoQueue(msg)
                else:
                    try:
                        nodeDest = getNode(dest, nodes)
                    except:
                        continue
                    if nodeSource.canCommunicate(nodeDest, chan):
                        nodeDest.pushDataIntoQueue(msg)
                        
            time.sleep(0.001)
        except KeyboardInterrupt or NameError:
            glovalVar.eventBreak.set()
            break
    pass


if __name__ == "__main__":
    main()

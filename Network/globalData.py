from buffer import Buffer
import configurations as conf
import threading
import time


buffers = [Buffer() for i in range(conf.NUM_NODES)]  # buffer of each mesh node
sharedBuffer = Buffer()  # buffer is shared between the mesh nodes
eventBreak = threading.Event()

def getCurrentTime():
    return time.time_ns()

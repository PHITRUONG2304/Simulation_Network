from buffer import Buffer
import configurations as conf
import threading
import time
import logging


buffers = [Buffer() for i in range(conf.NUM_NODES)]  # buffer of each mesh node
sharedBuffer = Buffer()  # buffer is shared between the mesh nodes
eventBreak = threading.Event()

# config log file
logging.basicConfig(filename="log.txt", filemode="w", level=logging.DEBUG, format="%(asctime)s %(message)s")

def _sleep(ms):
    _start = time.time_ns()
    while ((time.time_ns()) - _start) < ms:
        pass

def logFile(str):
    logging.debug(str)
    
def getCurrentTime():
    return time.time_ns()

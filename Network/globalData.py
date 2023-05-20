from buffer import Buffer
import configurations as conf
import threading
import time
import logging


sharedBuffer = Buffer()  # buffer is shared between the mesh nodes
eventBreak = threading.Event()

def setup_logger(name, fileName, level = logging.DEBUG, format="%(asctime)s %(message)s"):
    handler = logging.FileHandler(filename=fileName, mode="w")
    handler.setFormatter(logging.Formatter(format))
    
    logger = logging.getLogger(name=name)
    logger.setLevel(level)
    logger.addHandler(handler)
    return logger

# config log file
# log_1 = logging.basicConfig(filename="log.txt", filemode="w", level=logging.DEBUG, format=)
log_trace_communicate = setup_logger("trace communication", "communicate.txt")

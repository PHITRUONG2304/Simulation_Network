class Buffer:
    def __init__(self):
        self.blocked = False
        self.curSize = 0
        self.buffer = []
        pass

    def writeBuffer(self, data_array):
        self.blocked = True
        self.buffer.append(data_array)
        self.curSize += 1
        self.blocked = False
        pass

    def available(self):
        return self.curSize

    def canAccess(self):
        return not self.blocked

    def getBuffer(self):
        self.blocked = True
        data = []
        if self.available():
            data = self.buffer[0]
            self.buffer = self.buffer[1:]
            self.curSize -= 1
        self.blocked = False
        return data

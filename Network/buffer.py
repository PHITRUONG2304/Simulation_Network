class Buffer:
    def __init__(self):
        self.blocked = False
        self.buffer = []
        pass

    def writeBuffer(self, data_array):
        self.blocked = True
        self.buffer.append(data_array)
        self.blocked = False
        pass

    def available(self):
        return len(self.buffer)

    def canAccess(self):
        return not self.blocked

    def getBuffer(self):
        self.blocked = True
        data = []
        if self.available():
            data = self.buffer.pop()
        self.blocked = False
        return data

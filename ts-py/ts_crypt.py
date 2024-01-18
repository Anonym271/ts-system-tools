import struct
import numpy as np


class TSCrypt:
    N = 0x270 # 624
    ZERO = np.zeros(1, np.uint8)

    def __init__(self, seed=0xddcbacba):
        self.state = np.zeros(TSCrypt.N + 1, dtype=np.uint32)
        self.values = np.zeros(TSCrypt.N, dtype=np.uint8)
        self.counter = 0
        t1 = seed
        for i in range(TSCrypt.N):
            t2 = (t1 * 0x10dcd + 1) & 0xffffffff
            self.state[i] = (t1 & 0xffff0000) | (t2 >> 0x10)
            t1 = (t2 * 0x10dcd + 1) & 0xffffffff
        self.twist()

    
    def twist(self):
        state = self.state
        # for i in range(0xe3):
        #     y = (state[i+1] & 0x7fffffff) | (state[i] & 0x80000000)
        #     state[i] = (y & 1) * 0x9908b0df ^ y >> 1 ^ state[i + 0x18d]
        
        y = (state[1:0xe4] & 0x7fffffff) | (state[:0xe3] & 0x80000000)
        state[:0xe3] = (y & 1) * 0x9908b0df ^ y >> 1 ^ state[0x18d:0x270]

        state[-1] = state[0]
        
        # for i in range(0x18d):
        #     y = (state[i+0xe4] & 0x7fffffff) | (state[i+0xe3] & 0x80000000)
        #     state[i + 0xe3] = (y & 1) * 0x9908b0df ^ y >> 1 ^ state[i]

        y = (state[0xe4:] & 0x7fffffff) | (state[0xe3:0x270] & 0x80000000)
        # split this up to take the overlap into account
        y1 = y[:0xe3]
        y2 = y[0xe3:]
        state[0xe3:0x1c6] = (y1 & 1) * 0x9908b0df ^ y1 >> 1 ^ state[:0xe3]
        state[0x1c6:0x270] = (y2 & 1) * 0x9908b0df ^ y2 >> 1 ^ state[0xe3:0x18d]


        state = state[:-1]
        state = state >> 0xb ^ state
        state = ((state & 0x13a58ad) << 7 ^ state)
        state = ((state & 0x1df8c) << 0xf ^ state)
        self.values = (state >> 0x12 ^ state).astype(np.uint8)
        
        self.counter = 0


    def getKeys(self, count):
        results = []
        pos = 0

        # Extract the remaining keys
        if self.counter != 0:
            left = TSCrypt.N - self.counter
            if count < left:
                return self.values[self.counter:self.counter+count]
            results.append(self.values[self.counter:])
            self.twist()
            pos = left
        
        # Get as many blocks as possible
        left = count - pos
        nblocks = left // TSCrypt.N
        for i in range(nblocks):
            results.append(self.values)
            self.twist()
        pos += nblocks * TSCrypt.N
        
        # Get the remaining keys
        left = count - pos
        if left > 0:
            results.append(self.values[:left])
            self.counter = left
        
        return np.concatenate(results)


    def decrypt(self, data) -> np.ndarray[np.uint8, any]:
        if isinstance(data, np.ndarray):
            if data.dtype != np.uint8:
                data = data.astype(np.uint8)
        elif isinstance(data, bytes):
            data = np.frombuffer(data, dtype=np.uint8)
        else:
            data = np.array(data, dtype=np.uint8)
        
        keys = self.getKeys(len(data))
        return data ^ keys
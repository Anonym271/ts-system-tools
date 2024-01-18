from struct import Struct
from typing import Iterable

uint32 = Struct('<I')


TokenTypes = {
    0x00: 'identifier',
    0x01: 'number',
    0x02: '+',
    0x0a: '>=',
    0x0c: '==',
    0x12: '=',
    0x14: '&&'
}

TokenTypesInv = {v: k for k, v in TokenTypes.items()}


class VariableToken:
    def __init__(self, token_type: int | str, value: str):
        if type(token_type) is int:
            self.type = TokenTypes[token_type]
        else:
            self.type = token_type
        self.value = value
    
    @staticmethod
    def read(data, offset=0):
        t, = uint32.unpack(data[offset:offset+4])
        v = bytes(data[offset+4:offset+20]).split(b'\0')[0].decode('ascii')
        return VariableToken(t, v)
    
    def to_bytes(self):
        t = uint32.pack(TokenTypesInv[self.type])
        s = self.value.encode('ascii')
        z = b'\0' * (16 - len(s))
        return t+s+z



class VariableExpression:
    def __init__(self, tokens: Iterable[VariableToken]):
        self.tokens = tokens
    
    @staticmethod
    def read(data):
        return VariableExpression([VariableToken.read(data, pos) for pos in range(0, len(data), 20)])

    def to_bytes(self):
        return b''.join(t.to_bytes() for t in self.tokens)
    
    def write(self, stream):
        for t in self.tokens:
            stream.write(t.to_bytes())
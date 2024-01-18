from dataclasses import dataclass


@dataclass
class Opcode:
    opcode: int
    mnemonic: str
    arg_type: str | None = None
    encoding: str | None = None # deprecated

    def __str__(self):
        return self.mnemonic
    
    
    def __eq__(self, other):
        if type(self) is type(other):
            return self.opcode == other.opcode
        return False



OP = {
    0x0003: Opcode(0x0003, 'cmd', 'text', 'cp932'),
    0x0004: Opcode(0x0004, 'choice', 'text', 'ascii'),
    0x0007: Opcode(0x0007, 'speaker', 'text', 'cp932'),
    0x0032: Opcode(0x0032, 'text', 'text', 'cp932'),
    0x0045: Opcode(0x0045, 'return'),
    0x0048: Opcode(0x0048, 'clear'),
    0x005A: Opcode(0x005A, 'scene', 'text', 'ascii'),
    0x0065: Opcode(0x0065, 'event_cg', 'text', 'ascii'),
    0x0066: Opcode(0x0066, 'event_cg_', 'text', 'ascii'),
    0x0067: Opcode(0x0067, 'bg', 'text', 'ascii'),
    0x0069: Opcode(0x0069, '0069', 'number', 'text'),
    0x006B: Opcode(0x006B, 'char_speed', 'number', 'text'),
    0x006C: Opcode(0x006C, '006C', 'text', 'ascii'), # similar to char_sprite
    0x006D: Opcode(0x006D, '006D'),
    0x006E: Opcode(0x006E, 'if', 'expression'),
    0x0071: Opcode(0x0071, 'set', 'expression'),
    0x0072: Opcode(0x0072, 'jump'),
    0x0073: Opcode(0x0073, 'dialog', 'number', 'binary'),
    0x0075: Opcode(0x0075, '0075'),
    0x0076: Opcode(0x0076, '0076'),
    0x0077: Opcode(0x0077, 'sound'),
    0x0078: Opcode(0x0078, '0078'),
    0x0079: Opcode(0x0079, 'char_sprite'),
    0x007A: Opcode(0x007A, 'char_hide'),
    0x007B: Opcode(0x007b, 'char_sprite_'),
    0x007C: Opcode(0x007c, 'call'),
    0x0099: Opcode(0x0099, '0099'),
    0x009A: Opcode(0x009a, '009A'),
    0x00A0: Opcode(0x00a0, 'exit'),
    0x00A3: Opcode(0x00a3, 'voice'),
    0x00B4: Opcode(0x00b4, 'char_move'),
    0x00BB: Opcode(0x00bb, 'wait'),
    0x00E7: Opcode(0x00e7, 'video'),
    0x00E9: Opcode(0x00e9, 'sound_'),
    0x00EA: Opcode(0x00ea, '00EA'),
    0x00EB: Opcode(0x00eb, '00EB'),
    0x00EC: Opcode(0x00ec, '00EC'),
    0x00ED: Opcode(0x00ed, 'hide_col'),
    0x00EF: Opcode(0x00ef, '00EF'),
    0x00F0: Opcode(0x00f0, '00F0'),
    0x00F1: Opcode(0x00f1, '00F1'),
    0x00F5: Opcode(0x00f5, 'camera'),
    0x00F6: Opcode(0x00f6, '00F6'),
    0x00F7: Opcode(0x00f7, 'scene_show'),
    0x00F8: Opcode(0x00f8, 'scene_hide'),
    0x00FE: Opcode(0x00fe, 'date_a'),
    0x00FF: Opcode(0x00ff, 'date_b'),
    0x0101: Opcode(0x0101, 'map'),
    0x0102: Opcode(0x0102, '0102'),
    0x0103: Opcode(0x0103, '0103'),
    0x0104: Opcode(0x0104, '0104'),
    0x0105: Opcode(0x0105, '0105'),
    0x0106: Opcode(0x0106, '0106'),
    0x0107: Opcode(0x0107, '0107'),
    0x0108: Opcode(0x0108, '0108'),
    0x0109: Opcode(0x0109, '0109'),
    0x010A: Opcode(0x010a, '010A'),
    0x010B: Opcode(0x010b, 'title'),
    0x010C: Opcode(0x010c, '010C'),
}
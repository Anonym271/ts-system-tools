from struct import Struct, unpack, pack
import numpy as np
import logging as log

from ts_crypt import TSCrypt
from ts_opcodes import OP, Opcode
from ts_variable_expression import VariableExpression



# struct FILE {
#     struct NAMES {    
#         int stringCount;
#         struct STRING {
#             int nameLength;
#             char name[nameLength];
#         } stringNames[stringCount] <optimize=false>;
#     } names;
#
#     struct TABLES {
#         int tableCount;
#         struct TABLE {
#             int unk;
#             int entryCount1;
#             STRING command;
#             int entryCount2;
#             int entries[entryCount1];
#         } tables[tableCount] <optimize=false>;
#     } tables;
#
#     struct CONTENT {
#         int byteCount;
#         struct INSTRUCTION
#         {
#             int opcode;
#             STRING args;
#         } ;//<optimize=false>;
#         while(!FEof())
#             INSTRUCTION i;
#     } content;
# } file;


int8 = Struct('<b')
int16 = Struct('<h')
int32 = Struct('<i')
int64 = Struct('<q')
uint32 = Struct('<I')
uint32_2 = Struct('<II')


def read_ts_string(f, encoding='ascii'):
    count, = uint32.unpack(f.read(4))
    return f.read(count).decode(encoding)

def write_ts_string(s: str, f, encoding='ascii'):
    data = s.encode(encoding)
    f.write(uint32.pack(len(data)))
    f.write(data)



class Scene:
    def __init__(self, offset, name, n_frames, frame_offsets):
        self.offset = offset
        self.name = name
        self.n_frames = n_frames
        self.frame_offsets = frame_offsets

    @staticmethod
    def read(f):
        offset, n_frames = uint32_2.unpack(f.read(8)) # n_instructions is stored 2 times
        name = read_ts_string(f)
        n_frames, = uint32.unpack(f.read(4))
        data = unpack(f'<{n_frames}I', f.read(n_frames * 4))
        return Scene(offset, name, n_frames, data)
    
    def write(self, f):
        f.write(uint32_2.pack(self.offset, self.n_frames))
        write_ts_string(self.name, f)
        f.write(uint32.pack(self.n_frames))
        f.write(pack(f'<{self.n_frames}I', *self.frame_offsets))
    
    def __str__(self):
        return str(vars(self))



class Scenario:
    def __init__(self, variables: list[str], scenes: list[Scene], program: np.ndarray):
        self.variables = variables
        self.scenes = scenes
        self.program = program


    @staticmethod
    def load(filename, is_encrypted=True):
        log.info(f'Opening scenario file "{filename}"')
        with open(filename, 'rb') as f:
            log.debug('Loading variable definitions')
            n_vars, = uint32.unpack(f.read(4))
            variables = [read_ts_string(f) for i in range(n_vars)]

            log.debug('Loading scene definitions')
            n_scenes, = uint32.unpack(f.read(4))
            scenes = [Scene.read(f) for i in range(n_scenes)]

            # Program
            log.debug('Loading scenario program')
            program_length, = uint32.unpack(f.read(4))
            program_data = np.frombuffer(f.read(program_length), dtype=np.uint8)
            if is_encrypted:
                log.info('Decrypting scenario...')
                program_data = TSCrypt().decrypt(program_data)
                log.info('Decryption finished')
            program = program_data
        scenario = Scenario(variables, scenes, program)
        log.info(f'Successfully loaded scenario with {len(scenario.variables)} variables and {len(scenario.scenes)} scenes')
        return scenario


    def save(self, filename, encrypt=True):
        log.info(f'Saving scenario file "{filename}"')
        with open(filename, 'wb') as f:
            log.debug('Saving variable definitions')
            n_vars = len(self.variables)
            f.write(uint32.pack(n_vars))
            for v in self.variables:
                write_ts_string(v, f)
            
            log.debug('Saving scene definitions')
            n_scenes = len(self.scenes)
            f.write(uint32.pack(n_scenes))
            for scn in self.scenes:
                scn.write(f)

            log.debug('Saving scenario program')
            program_data = self.program
            if encrypt:
                log.info('Encryption scenario...')
                program_data = TSCrypt().decrypt(program_data)
                log.info('Encryption finished')
            program_length = len(program_data)
            f.write(uint32.pack(program_length))
            f.write(program_data)
        log.info('Saving done.')


class Instruction:
    MNEMONIC_LENGTH = 12

    def __init__(self, opcode: Opcode | int, args):
        if isinstance(opcode, Opcode):
            self.opcode = opcode
        else:
            self.opcode = OP[opcode]
        self.args = args
    

    @staticmethod
    def read(program, offset=0):
        data = program[offset:]
        opcode, = uint32.unpack(data[:4])
        if opcode == 0:
            return None, 0
        op = OP.get(opcode)
        if op is None:
            raise RuntimeError(f'Unknown opcode: {opcode:04X}')
        arglen, = uint32.unpack(data[4:8])
        if arglen > 0:
            args_raw = bytes(data[8:8+arglen])
            if op.arg_type is None:
                # log.warning(f'"{op.mnemonic}" received an undefined argument')
                log.debug(f'"{op.mnemonic}" received an undefined argument')
                args = args_raw
            elif op.arg_type == 'text':
                # Deprecated: cp932 works with everything
                # enc = 'ascii' if op.encoding is None else op.encoding
                enc = 'cp932'
                log.debug(f'Decoding with {enc}: {args_raw}')
                try:
                    args = args_raw.decode(enc)
                except:
                    log.error(f'Failed to decode text with {enc}, op = ({op.opcode:04X} / {op.mnemonic}, arg_type = {op.arg_type}, encoding = {op.encoding}), data={args_raw}')
                    args = args_raw
            elif op.arg_type == 'number':
                if op.encoding == 'binary':
                    if arglen == 1:
                        args = int8.unpack(args_raw)
                    elif arglen == 2:
                        args = int16.unpack(args_raw)
                    elif arglen == 4:
                        args = int32.unpack(args_raw)
                    elif arglen == 8:
                        args = int64.unpack(args_raw)
                    else:
                        raise ValueError(f'Tried to interpret binary data of length {arglen} as a number')
                elif op.encoding == 'text':
                    args = int(args_raw)
                else:
                    raise ValueError('Unknown number encoding: ' + op.encoding)
            elif op.arg_type == 'expression':
                args = VariableExpression.read(args_raw)
            else:
                log.warning('Unknown argument type: ' + op.arg_type)
                args = args_raw
        else:
            args = None
        return Instruction(op, args), 8 + arglen


    def _raw_args(self,  encoding='cp932') -> bytes | None:
        if self.args is None:
            return None
        if self.opcode.arg_type is None:
            # Raw binary args can be written directly into the stream
            return self.args
        elif self.opcode.arg_type == 'text':
            # Encode text arguments with the supplied encoding
            return self.args.encode(encoding)
        elif self.opcode.arg_type == 'number':
            if self.opcode.encoding == 'binary':
                # Just use 32 bits always...?
                return int32.pack(len(self.args))
            elif self.opcode.encoding == 'text':
                return str(self.args).encode(encoding)
            else:
                raise ValueError('Unknown number encoding: ' + self.opcode.encoding)
        elif self.opcode.arg_type == 'expression':
            return self.args.to_bytes()
        else:
            raise ValueError('Unknown argument type: ' + self.opcode.arg_type)


    def write(self, stream, encoding='cp932'):
        stream.write(uint32.pack(self.opcode.opcode))
        args = self._raw_args(encoding)
        if args is None:
            # No arguments -> arglen = 0
            stream.write(b'\0\0\0\0')
        else:
            stream.write(uint32.pack(len(args)))
            stream.write(args)


    def to_bytes(self, encoding='cp932'):
        args = self._raw_args(encoding)
        if args is None:
            return uint32.pack(self.opcode.opcode) + b'\0\0\0\0'
        return uint32.pack(self.opcode.opcode) + uint32.pack(len(args)) + args


    def pack_into(self, out_bytearray:bytearray, encoding='cp932') -> int:
        out_bytearray += uint32.pack(self.opcode.opcode)
        args = self._raw_args(encoding)
        if args is None:
            out_bytearray += b'\0\0\0\0'
            return 8
        else:
            out_bytearray += uint32.pack(len(args))
            out_bytearray += args
            return 8 + len(args)


    def args_escaped(self):
        if self.opcode.arg_type == 'text':
            return '"' + self.args.replace('\n', '\\n') + '"'
        else:
            return self.args

    
    def __str__(self):
        if self.args is None:
            return self.opcode.mnemonic
        return f'{self.opcode.mnemonic: <{Instruction.MNEMONIC_LENGTH}} {self.args_escaped()}'
        



if __name__ == '__main__':
    from argparse import ArgumentParser
    args = ArgumentParser()
    args.add_argument('input_file')
    args.add_argument('--output_file', '-o')
    args.add_argument('--encoding', '-e', default='utf-8', help="Encoding of *output* file")
    args.add_argument('--debug', '-d', action='store_true')
    args = args.parse_args()
    if args.debug:
        log.basicConfig(level=log.DEBUG)
    else:
        log.basicConfig(level=log.INFO)

    scenario = Scenario.load(args.input_file)
    if args.output_file:
        fout = open(args.output_file, 'w', encoding=args.encoding)
    else:
        fout = None
    pos = 0
    while True:
        ins, size = Instruction.read(scenario.program[pos:])
        if ins is None:
            break
        try:
            if fout:
                fout.write(str(ins) + '\n')
            else:
                print(ins)
        except Exception as err:
            log.error(err)
        pos += size
    log.info(f'Reached the end at position {pos:08X}')
    if fout:
        fout.close()
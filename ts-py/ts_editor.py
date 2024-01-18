import numpy as np
from dataclasses import dataclass
import ts_scenario
from ts_opcodes import OP
import pykakasi


OP_rev = {v.mnemonic : k for k, v in OP.items()}
return_codes = [OP_rev[c] for c in ['return', 'jump', 'exit']]



@dataclass
class Frame:
    instructions: list[ts_scenario.Instruction]

    @staticmethod
    def disassemble(program: np.ndarray, offset: int):
        instructions = []
        is_if = False
        pos = offset
        end = len(program)
        while pos < end:
            ins, size = ts_scenario.Instruction.read(program, pos)
            instructions.append(ins)
            op = ins.opcode.opcode
            if op is None:
                break
            # Ignore returns if last instruction was "if" 
            if not is_if and op in return_codes:
                break
            if op == 0x006e: # "if"
                is_if = True
            else:
                is_if = False
            pos += size
        return Frame(instructions)
    
    def assemble(self) -> bytes:
        res = bytearray()
        for ins in self.instructions:
            ins.pack_into(res)
        return bytes(res)


@dataclass
class Scene:
    name: str
    frames: list[Frame]



class NoScenarioLoadedException(Exception): 
    def __init__(self, *args):
        super().__init__('A scenario must be disassembled before calling assemble()', *args)


class ScenarioEditor:
    def __init__(self):
        self.loaded = False
        self.variables: list[str] = None
        self.scenes: list[Scene] = None

    def disassemble(self, scenario: ts_scenario.Scenario):
        # Resolve all references in address table
        prog = scenario.program
        scenes = []
        for scene in scenario.scenes:
            frames = [Frame.disassemble(prog, frame_offset) for frame_offset in scene.frame_offsets]
            scenes.append(Scene(scene.name, frames))
        self.variables = scenario.variables
        self.scenes = scenes
        self.loaded = True

    def assemble(self) -> ts_scenario.Scenario:
        if not self.loaded:
            raise NoScenarioLoadedException()
        program = bytearray()
        scenes: list[ts_scenario.Scene] = []
        pos = 0
        for scene in self.scenes:
            scene_offset = pos
            frame_offsets = []
            # First: write scene start (1x "scene" instruction with scene's name), don't include in list
            pos += ts_scenario.Instruction(0x005A, scene.name).pack_into(program)
            # Then: write scene's frames
            for frame in scene.frames:
                fdata = frame.assemble()
                program += fdata
                frame_offsets.append(pos)
                pos += len(fdata)
            scene = ts_scenario.Scene(scene_offset, scene.name, len(frame_offsets), tuple(frame_offsets))
            scenes.append(scene)
        program = np.array(program)
        return ts_scenario.Scenario(self.variables, scenes, program)

    def inject_furigana(self):
        if not self.loaded:
            raise NoScenarioLoadedException()
        fg = FuriganaGen()
        kks = pykakasi.kakasi()
        for scene in self.scenes:
            for frame in scene.frames:
                new_instruction_list = []
                is_already_ruby = False
                for ins in frame.instructions:
                    if is_already_ruby:
                        # Do not change anything until ruby ends
                        new_instruction_list.append(ins)
                        if ins.opcode.opcode == 0x0003 and ins.args.lower() == '</rb>':
                            is_already_ruby = False
                    else:
                        # If old ruby starts here: start is_already_ruby
                        if ins.opcode.opcode == 0x0003: # cmd
                            if ins.args.lower().startswith('<rb '):
                                # Ruby starts here
                                new_instruction_list.append(ins)
                                is_already_ruby = True
                        # If is text instruction: convert to furigana
                        elif ins.opcode.opcode == 0x0032: # text
                            new_instruction_list += fg.ts_furigana(ins)
                        # Everything else: just copy
                        else:
                            new_instruction_list.append(ins)
                frame.instructions = new_instruction_list




class FuriganaGen:
    def __init__(self):
        self.kks = pykakasi.kakasi()
    
    def furigana(self, text):
        segs = self.kks.convert(text)
        res = ''
        for seg in segs:
            if seg['orig'] in (seg['hira'], seg['kana']):
                res += seg['orig']
            else:
                res += f"{seg['orig']}[{seg['hira']}]"
        return res
    
    def ts_furigana(self, instruction: ts_scenario.Instruction) -> list[ts_scenario.Instruction]:
        new_instructions = []
        segments = self.kks.convert(instruction.args)
        text_to_keep = ''
        for seg in segments:
            if seg['orig'] in (seg['hira'], seg['kana']):
                # No furigana needed
                text_to_keep += seg['orig']
            else:
                # Clear text_to_keep and add furigana to this
                if text_to_keep:
                    new_instructions.append(ts_scenario.Instruction(0x0032, text_to_keep))
                    text_to_keep = ''
                new_instructions.append(ts_scenario.Instruction(0x0003, f'<RB "{seg["hira"]}">'))
                new_instructions.append(ts_scenario.Instruction(0x0032, seg['orig']))
                new_instructions.append(ts_scenario.Instruction(0x0003, '</rb>'))
        # Finally add all remaining text_to_keep
        if text_to_keep:
            new_instructions.append(ts_scenario.Instruction(0x0032, text_to_keep))
        return new_instructions


if __name__ == '__main__':
    import argparse
    from argparse import ArgumentParser
    from pathlib import Path

    def str2bool(v):
        if isinstance(v, bool):
            return v
        if v.lower() in ('yes', 'true', 't', 'y', '1'):
            return True
        elif v.lower() in ('no', 'false', 'f', 'n', '0'):
            return False
        else:
            raise argparse.ArgumentTypeError('Boolean value expected.')
    
    args = ArgumentParser()
    args.add_argument('--furigana', help='Add furigana to all kanji (in spoken text)', action='store_true')
    args.add_argument('--output', '-o', help='Output file', default='out.dat')
    args.add_argument('--decrypt', '-d', help='Does input file need to be decrypted? (Default: True)', type=str2bool, default=True)
    args.add_argument('-D', dest='decrypt', action='store_false', help='Short for "--decrypt false"')
    args.add_argument('--encrypt', '-e', help='Encrypt output file? (Default: True)', type=str2bool, default=True)
    args.add_argument('-E', dest='encrypt', action='store_false',  help='Short for "--encrypt false"')
    args.add_argument('input_file', help='Input file (scenario.dat)')
    args = args.parse_args()

    in_fn = Path(args.input_file)
    out_fn = Path(args.output)

    any_actions = args.furigana # or ... for future actions
    if not any_actions:
        print('No actions were selected to perform on the scenario file.')
        exit(1)

    print('Loading Scenario')
    try:
        scenario_orig = ts_scenario.Scenario.load(in_fn, args.decrypt)
    except FileNotFoundError:
        print(f'Error: could not open input file "{in_fn}"')
        exit(1)
    except Exception as exc:
        print(f'Failed to load scenario: {exc}')
        exit(1)
    print('Disassembling Scenario')
    editor = ScenarioEditor()
    try:
        editor.disassemble(scenario_orig)
    except Exception as exc:
        print(f'Failed to load scenario: "{exc}". Maybe it is {"already decrypted" if args.decrypt else "encrypted"}?')
        exit(1)

    if args.furigana:
        print('Injecting furigana (this may take a while...)')
        editor.inject_furigana()

    print('Reassembling')
    scenario_new = editor.assemble()
    print('Saving scenario to', out_fn)
    scenario_new.save(out_fn, args.encrypt)

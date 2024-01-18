# ts-py
This is a shitty but working wannabe python library for dealing with the scenario.dat files of the TS-System VN engine. Currently it has the following features:
- **Decrypting and encrypting** of scenario.dat files (using a numpy implementation of the mersenne twister algorithm that I am a little bit proud of)
- **Disassembling** scenario.dat files **only into python structures, not into code**. In order to edit the assembly code in a text editor use the other (C++ based) tools in this repository.
- **Reassembling** scenario.dat files (again, just internally)
- Generating **furigana** for all the kanji in the spoken text lines and injecting them into scenario.dat (because my Japanese is kinda basic lul)

With custom python code it is of course also possible to edit all the scripts freely because the disassembling and reassembling seems to work flawlessly. There is just no "export into text file" and "import from text file". This could be done relatively easily using json or yaml. However, as I said: if you want to edit the scripts by hand you are better off with my C++ disassembler and assembler. This python lib is made for **automated** working with the scripts (furigana, machine translations, ...).

## Usage
Currently, the main file is `ts_editor.py`. It has only one function:
### Generate furigana
```python ts_editor.py --furigana [-D] [-E] [-o output.dat] scenario.dat```
Use this command to create furigana for the file `scenario.dat`. Options:
- `-o output.dat`: Output file. Default is `out.dat`
- `-D`: do not decrypt the input file (because it is already stored in decrypted form)
- `-E`: do not encrypt the output file when saving

Default behaviour is to use decryption and encryption since this is the way the game files are found naturally.

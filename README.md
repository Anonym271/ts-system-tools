# ts-system-tools
A collection of handmade tools for the VN engine I call "TS System" (used in the eroge "Tropical Kiss")

This repository currently contains:
* A python script for unpacking and repacking the resource archives (.pak)
* A C++ program to decrypt the Scenario.dat file
* A C++ program to disassemble the Scenario.dat into readable, assembly-like code (I call this code TSA for TS assembly)
* A C++ program to reassemble modificated scenario files into a .dat file that the game accepts

## Building
I am sorry to say that I have not yet put much effort into a comfortable build system or anything, for now the C++ projects are just regular VS project files. You will have to tweak the settings for the individual projects by yourself (especially mind the include paths, library paths and used dlls). The good news is that the program currently consists of very few files, so even building with g++ and command line should not be too hard to achieve.

If you are using Win64, you should try the binary release though. I didn't try it on another pc, but it should work. Please create an issue if it doesn't.

External libraries I am using:
* ICU for converting between Shift-JIS and UTF-8 encoding
* RE:flex for the unicode supporting lexical analyzer

# Disclaimer
Everything is highly experimental, especially the TSA-assembler. I have not yet fully decoded all the opcodes, but for now everything I tried worked fine. If you have ideas or problems, feel free to contact me, open an issue or modify the code in any way you like.

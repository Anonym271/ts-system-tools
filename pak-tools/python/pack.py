import os
import struct
import sys
import numpy as np
from collections import namedtuple

FileTableEntry = namedtuple("FileTableEntry", "name offset info")

def inverseHse(data):
    tmp = -np.frombuffer(data, dtype='b')
    tmp = np.where(tmp == 128, -128, tmp)
    return np.ndarray.tobytes(tmp)

def extractFile(file, offset, length, filename):
    print(filename)

    file.seek(offset)
    data = file.read(length)
    
    if filename.endswith(".hse"):
        data = inverseHse(data)
    
    out = open(filename, "wb")
    out.write(data)

def readTable(file, offset):
    file.seek(offset)
    dirCount = struct.unpack('I', file.read(4))[0]
    dirs = [FileTableEntry] * dirCount
    for i in range(dirCount):
        raw = struct.unpack('32sII', file.read(40))
        dirs[i] = FileTableEntry(raw[0].decode('ascii').split('\0')[0], raw[1], raw[2])
    fileCount = struct.unpack('i', file.read(4))[0]
    files = [FileTableEntry] * fileCount
    for i in range(fileCount):
        raw = struct.unpack('32sII', file.read(40))
        files[i] = FileTableEntry(raw[0].decode('ascii').split('\0')[0], raw[1], raw[2])
    return dirs, files

def extractDir(file, tableOffset, dataOffset, path):
    os.makedirs(path, exist_ok=True)

    subdirs, files = readTable(file, tableOffset)
    for dir in subdirs:
        subpath = path + '/' + dir.name
        extractDir(file, dir.offset, dir.info, subpath)

    for f in files:
        fullname = path + '/' + f.name
        extractFile(file, dataOffset + f.offset, f.info, fullname)

def extractArchive(arcname, outDir):
    f = open(arcname, "rb")
    magic = struct.unpack('4s', f.read(4))[0]
    if magic != b"PACK":
        raise Exception("Invalid PACK header!")
    rootTable = 8
    rootData = struct.unpack('i', f.read(4))[0]
    extractDir(f, rootTable, rootData, outDir)


def packFile(pakFile, filename):
    file = open(filename, 'rb')
    data = file.read()
    if filename.endswith('.hse'):
        data = inverseHse(data)
    pakFile.write(data)
    return len(data)

def packDir(pakFile, dirname):
    subdirs = []
    files = []
    for entry in os.scandir(dirname):
        if entry.is_dir(follow_symlinks=False):
            #print(dirname + "/" + entry.name)
            subdirs.append(entry.name)
        else:
            files.append(entry.name)

    entryCount = len(subdirs) + len(files)
    thisTableSize = entryCount * 40 + 8
    thisTableOffset = pakFile.tell()
    thisDataOffset = thisTableOffset + thisTableSize
    
    subdirEntries = []
    fileEntries = []

    pakFile.seek(thisDataOffset)
    offset = 0
    for file in files:
        length = packFile(pakFile, dirname + '/' + file)
        fileEntries.append(struct.pack('32sII', file.encode('ascii'), offset, length))
        offset += length
        
    for subdir in subdirs:
        subTable, subData = packDir(pakFile, dirname + '/' + subdir)
        subdirEntries.append(struct.pack('32sII', subdir.encode('ascii'), subTable, subData))

    offset = pakFile.tell()
    pakFile.seek(thisTableOffset)
    pakFile.write(struct.pack('I', len(subdirs)))
    for entry in subdirEntries:
        pakFile.write(entry)
    pakFile.write(struct.pack('I', len(files)))
    for entry in fileEntries:
        pakFile.write(entry)
    pakFile.seek(offset)

    return thisTableOffset, thisDataOffset
    
def packArchive(inDir, outFile):
    f = open(outFile, 'wb')
    f.write(b'PACK')
    f.seek(8)
    _, dataOffs = packDir(f, inDir)
    f.write(struct.pack('I', dataOffs))



def printUsage():
    print("usage: <me> extract <input.pak> [output-dir]")
    print("       <me> pack <input-dir> [output.pak]")
    exit(1)

def main(argc, argv):
    if argc == 3:
        if argv[1] == "extract":
            extractArchive(argv[2], argv[2][:argv[2].rfind('.')])
        elif argv[1] == "pack":
            packArchive(argv[2], argv[2] + ".pak")
        else:
            printUsage(); 
    elif argc == 4:
        if argv[1] == "extract":
            extractArchive(argv[2], argv[3])
        elif argv[1] == "pack":
            packArchive(argv[2], argv[3])
    else:
        printUsage()

if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
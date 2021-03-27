#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdint>

#include "KeyTable.hpp"

typedef KeyTable keyTable_t;

void readPartOne(std::ifstream& file)
{
    // dummy, just skip that part
    uint32_t count;
    uint32_t size;
    file.read((char*)&count, 4);
    for (int i = 0; i < count; i++)
    {
        file.read((char*)&size, 4);
        file.seekg(size, std::ios::cur);
        if (!file.good())
        {
            std::cout << "IO error in part 1, item " << i << "!\n";
            exit(1);
        }
    }
}
void readPartTwo(std::ifstream& file)
{
    // dummy, just skip that part
    uint32_t count;
    uint32_t size;
    file.read((char*)&count, 4);
    for (int i = 0; i < count; i++)
    {
        file.seekg(8, std::ios::cur);
        file.read((char*)&size, 4);
        file.seekg(size, std::ios::cur);
        if (!file.good())
        {
            std::cout << "IO error in part 2, item " << i  << " while skipping name!\n";
            exit(1);
        }
        file.read((char*)&size, 4);
        size *= 4;
        file.seekg(size, std::ios::cur);
        if (!file.good())
        {
            std::cout << "IO error in part 2, item " << i  << " while skipping data!\n";
            exit(1);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: <me> <input-file>\nOutput: <input-file>_dec.dat with decrypted scenario program.\n";
        return 0;
    }

    std::string fname(argv[1]);

    std::ifstream file(fname, std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Could not open file '" << fname << "'\n";
        return 1;
    }

    size_t dotpos = fname.find_last_of('.');
    if (dotpos >= 0)
    {
        std::string ext = fname.substr(dotpos);
        fname = fname.substr(0, dotpos) + "_dec" + ext;        
    }
    std::ofstream out(fname, std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Could not open file '" << fname << "'\n";
        return 1;
    }

    keyTable_t keyTable(0xddcbacba);

    readPartOne(file); // read the strings
    readPartTwo(file); // read the integers

    auto data_start = file.tellg(); 
    std::vector<char> data(data_start);
    file.seekg(0);
    file.read(data.data(), data_start);
    out.write(data.data(), data_start);

    uint32_t data_length;
    file.read((char*)&data_length, 4);
    out.write((char*)&data_length, 4);
    
    if (!data_length)
    {
        std::cout << "Invalid scenario file!\n";
        return 1;
    }
    auto decryptedData = new uint8_t[data_length];

    for (int i = 0; i < data_length; i++, keyTable.counter++) 
    {
        if (keyTable.counter > 0x26f)
            keyTable.update();
        
        int key = keyTable.externalKeys[keyTable.counter];
        int byte = file.get();
        if (byte < 0)
        {
            std::cout << "Error: unexpected OEF!\n";
            return 1;
        }
        uint8_t dec = (uint8_t)(byte ^ key);
        out.write((char*)&dec, 1);
    }

    return 1;
}
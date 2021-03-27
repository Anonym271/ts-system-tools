#include "KeyTable.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

KeyTable::KeyTable(int seed)
{
    std::cout << "Layout:" << std::hex
        << "\n\tcounter:      0x" << (size_t)&counter     - (size_t)this
   //     << "\n\tval_0004:     0x" << (size_t)&val_0004    - (size_t)this
        << "\n\tinternalKeys: 0x" << (size_t)internalKeys - (size_t)this
        << "\n\texternalKeys: 0x" << (size_t)externalKeys - (size_t)this
        << "\n\tval_1388:     0x" << (size_t)&val_1388    - (size_t)this
        << std::endl;

    uint32_t *keys;
    uint32_t uVar4;
    uint32_t uVar5;
    std::memset(internalKeys, 0, 624 * 4);
    std::memset(externalKeys, 0, 624 * 4);
    keys = internalKeys + 1;
    for (int i = 0x68; i != 0; i--) {
        uVar4 = seed * 0x10dcd + 1;
        keys[-1] = seed & 0xffff0000;
        keys[-1] = keys[-1] | uVar4 >> 0x10;
        uVar4 = uVar4 * 0x10dcd + 1;
        uVar5 = uVar4 * 0x10dcd + 1;
        *keys = uVar4 & 0xffff0000;
        *keys = *keys | uVar5 >> 0x10;
        uVar4 = uVar5 * 0x10dcd + 1;
        uVar5 = uVar4 * 0x10dcd + 1;
        keys[1] = uVar4 & 0xffff0000;
        keys[1] = keys[1] | uVar5 >> 0x10;
        uVar4 = uVar5 * 0x10dcd + 1;
        uVar5 = uVar4 * 0x10dcd + 1;
        keys[2] = uVar4 & 0xffff0000;
        keys[2] = keys[2] | uVar5 >> 0x10;
        uVar4 = uVar5 * 0x10dcd + 1;
        uVar5 = uVar4 * 0x10dcd + 1;
        keys[3] = uVar4 & 0xffff0000;
        keys[3] = keys[3] | uVar5 >> 0x10;
        uVar4 = uVar5 * 0x10dcd + 1;
        uVar5 = uVar4 * 0x10dcd + 1;
        keys[4] = uVar4 & 0xffff0000;
        keys[4] = keys[4] | uVar5 >> 0x10;
        seed = uVar5 * 0x10dcd + 1;
        keys += 6;
    }
  //  val_0004 = 0;

// #ifdef NDEBUG
//     std::string fname("init_table.dat");
// #else
//     std::string fname("init_table_d.dat");
// #endif
//     std::ofstream f(fname, std::ios::binary);
//     f.write((char*)internalKeys, sizeof(internalKeys));
//     f.close();

    val_1388 = 1;
    update();
    return;
}

void KeyTable::update()
{
    uint32_t uVar1;
    uint32_t uVar2;
    uint32_t uVar4;

    uint32_t* internalKeys_1 = internalKeys + 1;

    uVar2 = internalKeys[0];
    uVar4 = internalKeys_1[0];
    for (int i = 0; i < 0xe3; i++)
    {
        uVar1 = uVar2 & 0x80000000;
        uVar2 = internalKeys_1[i];
        internalKeys_1[i - 1] =
            (uVar4 & 0x7fffffff | 0xfffffffe) + 1 & 0x9908b0df ^
            (uVar1 | uVar4 & 0x7fffffff) >> 1 ^ internalKeys_1[i + 0x18c] ^ 0x9908b0df;
        uVar4 = internalKeys_1[i + 1];
    }
    internalKeys_1[0x26f] = internalKeys[0];
    uVar2 = internalKeys_1[0xe2];
    uVar4 = internalKeys_1[0xe3];
    for (int i = 0; i < 0x18d; i++)
    {
        uVar1 = uVar2 & 0x80000000;
        uVar2 = internalKeys_1[i + 0xe3];
        internalKeys_1[i + 0xe2] =
            (uVar4 & 0x7fffffff | 0xfffffffe) + 1 & 0x9908b0df ^
            (uVar1 | uVar4 & 0x7fffffff) >> 1 ^ internalKeys_1[i - 1] ^ 0x9908b0df;
        if (i < 0x18c) // because g++ complains about "undefined behaviour in iteration 0x18c" else
            uVar4 = internalKeys_1[i + 0xe4];
    }
    uVar2 = internalKeys[0];
    for (int i = 0; i < 0x138; i++)
    {
        uVar2 = uVar2 >> 0xb ^ uVar2;
        uVar4 = internalKeys_1[i * 2] >> 0xb ^ internalKeys_1[i * 2];
        uVar2 = (uVar2 & 0x13a58ad) << 7 ^ uVar2;
        uVar4 = (uVar4 & 0x13a58ad) << 7 ^ uVar4;
        uVar2 = (uVar2 & 0x1df8c) << 0xf ^ uVar2;
        uVar4 = (uVar4 & 0x1df8c) << 0xf ^ uVar4;
        externalKeys[i * 2] = uVar2 >> 0x12 ^ uVar2;
        externalKeys[i * 2 + 1] = uVar4 >> 0x12 ^ uVar4;
        uVar2 = internalKeys_1[i * 2 + 1];
    }
    counter = 0;
    return;
}
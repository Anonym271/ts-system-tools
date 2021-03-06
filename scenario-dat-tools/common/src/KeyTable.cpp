#include "../include/tss/KeyTable.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

namespace tss
{
    KeyTable::KeyTable(uint32_t seed)
    {
        uint32_t* keys;
        uint32_t uVar4;
        uint32_t uVar5;

        std::memset(internalKeys, 0, 625 * 4);
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
    }

    void KeyTable::decrypt(char* data, size_t length)
    {
        for (int i = 0; i < length; i++, counter++)
        {
            if (counter > 0x26f)
                update();
            int key = externalKeys[counter];
            int byte = data[i];
            data[i] = (uint8_t)(byte ^ key);
        }
    }
}
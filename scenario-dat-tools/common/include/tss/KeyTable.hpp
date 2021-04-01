#pragma once

#include <cstdint>
#include <cstdlib>

namespace tss
{
    struct KeyTable
    {
    public:
        uint32_t counter;
    private:
        uint32_t internalKeys[625];
    public:
        uint32_t externalKeys[624];
        uint32_t val_1388;

    public:
        KeyTable(uint32_t seed = 0xddcbacba);

        void decrypt(char* data, size_t length);

    private:
        void update();
    };
}
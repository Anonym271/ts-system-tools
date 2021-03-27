#include <cstdint>

#pragma pack(push, 1)
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
    KeyTable(int seed);
    void update();
};
#pragma pack(pop)
#include "../include/tss/Scenario.hpp"

namespace tss
{
    Scenario Scenario::load(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        file.exceptions();

        uint32_t count;
        uint32_t length;

        std::vector<Scene> scenes;
        std::vector<std::string> variables;
        std::vector<char> data;

        file.read((char*)&count, 4);
        variables.reserve(count);
        for (int i = 0; i < count; i++)
        {
            file.read((char*)&length, 4);
            std::string& str = variables.emplace_back(length, '\0');
            file.read(str.data(), length);
        }

        uint32_t ch_offset;
        uint32_t ch_inst_count;
        std::string ch_name;
        std::vector<uint32_t> ch_data;
        file.read((char*)&count, 4);
        scenes.reserve(count);
        for (int i = 0; i < count; i++)
        {
            file.read((char*)&ch_offset, 4);
            file.seekg(4, std::ios::cur); // skip duplicate instruction count
            file.read((char*)&length, 4);
            ch_name.resize(length, '\0');
            file.read(ch_name.data(), length);
            file.read((char*)&ch_inst_count, 4);
            ch_data.resize(ch_inst_count);
            file.read((char*)ch_data.data(), ch_inst_count * 4);
            scenes.emplace_back(std::move(ch_name), std::move(ch_data), ch_offset);
        }

        file.read((char*)&count, 4);
        std::vector<char> binary_data(count);
        file.read(binary_data.data(), count);
        KeyTable keys;
        keys.decrypt(binary_data.data(), binary_data.size());

        return Scenario(std::move(scenes), std::move(binary_data), std::move(variables));
    }
}
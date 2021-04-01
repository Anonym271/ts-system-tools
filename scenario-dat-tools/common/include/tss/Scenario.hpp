#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>

#include "KeyTable.hpp"

namespace tss
{
    struct Scene
    {
    private:
        std::string _name;
        std::vector<uint32_t> _data;
        uint32_t _offest0;

    public:
        Scene(const std::string& name, const std::vector<uint32_t>& data, uint32_t offset) :
            _name(name), _data(data), _offest0(offset)
        {}
        Scene(const std::string&& name, std::vector<uint32_t>&& data, uint32_t offset) :
            _name(name), _data(data), _offest0(offset)
        {}
        Scene(const Scene& other) :
            _name(other._name), _data(other._data), _offest0(other._offest0)
        {}
        Scene(Scene&& old) noexcept :
            _name(std::move(old._name)), _data(std::move(old._data)), _offest0(std::move(old._offest0))
        {}

        const std::string& name() const { return _name; }
        std::string& name() { return _name; }
        void name(const std::string& new_name) { _name = new_name; }
        const std::vector<uint32_t>& data() const { return _data; }
        std::vector<uint32_t>& data() { return _data; }
        uint32_t offset() const { return _offest0; }

        uint32_t operator[](size_t index) const { return _data[index]; }
    };

    struct Instruction
    {
        uint32_t opcode;
        std::vector<char> data;

        Instruction(uint32_t opcode, const std::vector<char>& data) :
            opcode(opcode), data(data)
        {}
        Instruction(uint32_t opcode, std::vector<char>&& data) :
            opcode(opcode), data(data)
        {}
        Instruction(const Instruction& other) :
            opcode(other.opcode), data(other.data)
        {}
        Instruction(Instruction&& other) noexcept :
            opcode(other.opcode), data(std::move(other.data))
        {}
    };

    class Scenario
    {
    private:
        std::vector<std::string> _variables;
        std::vector<Scene> _scenes;
        std::vector<char> _binaries;

    public:
        Scenario(const std::vector<Scene>& scenes,
            const std::vector<char>& binaries,
            const std::vector<std::string>& variables) :
            _variables(variables), _scenes(scenes), _binaries(binaries)
        {}
        Scenario(std::vector<Scene>&& scenes,
            std::vector<char>&& binaries,
            std::vector<std::string>&& variables) :
            _variables(variables), _scenes(scenes), _binaries(binaries)
        {}
        Scenario(const Scenario& other) :
            _variables(other._variables), _scenes(other._scenes), _binaries(other._binaries)
        {}
        Scenario(Scenario&& other) noexcept :
            _variables(std::move(other._variables)),
            _scenes(std::move(other._scenes)),
            _binaries(std::move(other._binaries))
        {}

        static Scenario load(const std::string& filename);

        const std::vector<Scene>& scenes() const { return _scenes; }
        std::vector<Scene>& scenes() { return _scenes; }
        const std::vector<char>& binaries() const { return _binaries; }
        std::vector<char>& binaries() { return _binaries; }
        const std::vector<std::string>& variables() const { return _variables; }
        std::vector<std::string>& variables() { return _variables; }

        const char* operator[](size_t index) const { return &_binaries[index]; }
        char* operator[](size_t index) { return &_binaries[index]; }
    };
}
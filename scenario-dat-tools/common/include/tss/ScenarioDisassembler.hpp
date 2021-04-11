#pragma once
#include <cstdint>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
//#include <unordered_set>

#include "Scenario.hpp"
#include "ShiftJIS.hpp"
#include "Opcode.hpp"

namespace tss
{
    class ScenarioDisassembler
    {
    public:
        enum Options
        {
            NO_OPTIONS = 0,
            ONE_FILE = 0b0001,
            DEBUG = 0b0010,
            UTF8 = 0b0100,
        };

        Options options;

        ScenarioDisassembler(Options options = NO_OPTIONS) :
            options(options)
        { }
        
        void disassemble_raw(const std::string& in_filename, const std::string& out_filename)
        {
            auto scenario = Scenario::load(in_filename);
            disassemble_raw(scenario, out_filename);
        }
        void disassemble_raw(const Scenario& scenario, const std::string& out_filename)
        {
            std::unordered_map<uint32_t, std::string> frames;
            for (const Scene& scn : scenario.scenes())
                frames.insert(std::make_pair(scn.offset(), scn.name()));

            std::map<uint32_t, std::pair<std::string, std::string>> additional_jumps;
            for (const Scene& scn : scenario.scenes())
            {
                std::string name;
                int i = 0;
                for (uint32_t offset : scn.data())
                {
                    name = scn.name() + "." + std::to_string(i);
                    auto ins_res = frames.insert(std::make_pair(offset, name));
                    if (!ins_res.second)
                    {
                        std::cout << "Info: offset 0x" << std::hex << std::setw(8) << std::setfill('0') << offset
                            << ": frame " << std::dec << i << " of scene " << scn.name()
                            << " collides with base address of scene " << ins_res.first->second
                            << ". Inserting additional jump statement.\n";
                        additional_jumps.insert(std::make_pair(offset, std::make_pair(name, ins_res.first->second)));
                    }
                    frames.insert(std::make_pair(offset, name));
                    i++;
                }
            }

            std::ofstream file(out_filename);
            file << std::hex;
            int arglen;
            int opcode;
            const char* start = scenario.binaries().data();
            uint32_t end = scenario.binaries().size();
            uint32_t pos = 0;
            auto add_it = additional_jumps.begin(), add_end = additional_jumps.end();
            while (pos < end)
            {
                if (add_it != add_end && pos == add_it->first)
                {
                    // additional jump at this position
                    file << "# " << add_it->second.first << ":\n";
                    file << "\t(" << std::right << std::setfill('0') << std::setw(8) << pos << std::setfill(' ') << std::left << ")   ";
                    write_opcode(file, Opcode::JUMP);
                    write_arg(file, Opcode::JUMP, add_it->second.second.data(), add_it->second.second.length());
                    file.put('\n');
                    ++add_it;
                }

                auto it = frames.find(pos);
                if (it != frames.end())
                    file << "# " << it->second << '\n';
                file << "\t(" << std::right << std::setfill('0') << std::setw(8) << pos << std::setfill(' ') << std::left << ")   ";

                opcode = *(int*)(start + pos);
                pos += 4;

                if (opcode == 0)
                    break;
                arglen = *(int*)(start + pos);
                pos += 4;
                write_opcode(file, (Opcode)opcode);
                if (arglen > 0)
                {
                    write_arg(file, (Opcode)opcode, (start + pos), arglen);
                    pos += arglen;
                }

                file.put('\n');
            }
        }

    private:
        void write_arg(std::ostream& file, Opcode opcode, const char* arg, size_t length)
        {
            file.put('"');
            switch (opcode)
            {
            case Opcode::IF:
            case Opcode::SET:
            {
                const char* pos = arg + 4;
                while (true)
                {
                    file << pos;
                    pos += 20;
                    if (pos < arg + length)
                        file << ' ';
                    else break;
                }
                break;
            }
            case Opcode::SPEAKER:
            case Opcode::TEXT:
            case Opcode::TITLE:
            case Opcode::X_0102:
            case Opcode::META:
                if (options & UTF8)
                {
                    auto utf8 = ShiftJIS::to_utf8_esc(arg, length);
                    file.write(utf8.data(), utf8.size());
                }
                else
                {
                    file.write(arg, length);
                }
                break;
            case Opcode::CHOOSE:
                for (int i = 0; i < length; i++)
                {
                    file << "\\x" << std::hex << +arg[i];
                }
                break;
            default:
                file.write(arg, length);
            }
            file.put('"');
        }

        void write_frame(std::ostream& file, const char* pos)
        {
            Opcode opcode;
            uint32_t arglen;
            bool returned = false;
            while (!returned)
            {
                opcode = *reinterpret_cast<const Opcode*>(pos);
                pos += 4;
                arglen = *reinterpret_cast<const uint32_t*>(pos);
                pos += 4;
                file.put('\t');
                write_opcode(file, opcode);
                if (opcode == Opcode::RETURN || opcode == Opcode::JUMP || opcode == Opcode::EXIT)
                    returned = true;
                if (arglen != 0)
                {
                    write_arg(file, opcode, pos, arglen);
                    pos += arglen;
                }
                file.put('\n');
            }
        }

        void write_scene(std::ostream& file, const Scenario& scenario, const Scene& scene)
        {
            const char* base = scenario.binaries().data();
            write_frame(file, base + scene.offset());

            file << scene.name() << ":\n";

            for (uint32_t offs : scene.data())
                write_frame(file, base + offs);

            file.put('\n');
        }

    };
}
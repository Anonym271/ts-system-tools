#pragma once
#include <cstdint>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>

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
        {

        }

        void disassemble(const std::string& filename, const std::string& out_dir)
        {
            auto scenario = Scenario::load(filename);
            disassemble(scenario, out_dir);
        }

        void disassemble(const Scenario& scenario, const std::string& out_dir)
        {
            namespace fs = std::filesystem;
            fs::path dirpath(out_dir);
            fs::create_directories(dirpath);
            if (options & DEBUG)
                std::cout << "Created directory " << dirpath << std::endl;

            fs::path current_path(dirpath / "main.tsa");
            if (options & DEBUG)
                std::cout << "> " << current_path << std::endl;
            std::ofstream main_file(current_path);
            main_file << "/encoding shift-jis\n\n";
            for (auto& var : scenario.variables())
                main_file << "/def " << var << '\n';

            std::multimap<std::string, Scene> scenes;
            for (auto& scene : scenario.scenes())
            {
                int at_pos = scene.name().find('@');
                if (at_pos >= 0)
                {
                    scenes.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(scene.name().substr(0, at_pos)),
                        std::forward_as_tuple(scene.name().substr(at_pos + 1), scene.data(), scene.offset()));
                }
                else
                {
                    scenes.emplace(scene.name(), scene);
                }
            }


            auto link_it = scenes.find("_link");
            if (link_it != scenes.end())
            {
                main_file << '\n' << std::hex << std::left;
                do {
                    if (link_it->second.name() == "_link")
                        link_it->second.name("main");
                    write_scene(main_file, scenario, link_it->second);
                    scenes.erase(link_it++);
                } while (link_it != scenes.end() && link_it->first == "_link");
                main_file << std::dec;
            }

            std::ofstream file;
            file << std::left;

            for (auto it = scenes.begin(); it != scenes.end();)
            {
                const std::string& name = it->first;

                current_path = dirpath / (name + ".tsa");
                main_file << "\n/import " << current_path.filename();
                if (options & DEBUG)
                    std::cout << "> " << current_path << std::endl;
                file.open(current_path);

                do {
                    write_scene(file, scenario, it->second);
                    ++it;
                } while (it != scenes.end() && name == it->first);
                file.close();
            }

            if (options & DEBUG)
                std::cout << "Done.\n";
        }

        void disassemble_raw(const std::string& in_filename, const std::string& out_filename)
        {
            auto scenario = Scenario::load(in_filename);
            disassemble_raw(scenario, out_filename);
        }
        void disassemble_raw(const Scenario& scenario, const std::string& out_filename)
        {
            std::unordered_map<uint32_t, std::string> frames;
            for (const Scene& scn : scenario.scenes())
            {
                frames.insert(std::make_pair(scn.offset(), scn.name()));
                int i = 0;
                for (uint32_t offset : scn.data())
                    frames.insert(std::make_pair(offset, "# " + scn.name() + "." + std::to_string(i++)));
            }

            std::ofstream file(out_filename);
            file << std::hex;
            int arglen;
            int opcode;
            const char* start = scenario.binaries().data();
            uint32_t end = scenario.binaries().size();
            uint32_t pos = 0;
            while (pos < end)
            {
                auto it = frames.find(pos);
                if (it != frames.end())
                    file << it->second << ":\n";
                opcode = *(int*)(start + pos);
                pos += 4;
                if (opcode == 0)
                    break;
                arglen = *(int*)(start + pos);
                pos += 4;
                file << "\t(" << std::right << std::setfill('0') << std::setw(8) << pos << std::setfill(' ') << std::left << ")   ";
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
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <list>

#include <tss/Opcode.hpp>
#include "lex.yy.h"

namespace tss
{
	class Parser
	{
		using Opc = tss::Opcode;

		struct Scene
		{
			uint32_t offset;
			std::string name;
			std::vector<uint32_t> frames;

			Scene(uint32_t off, const std::string& n) :
				offset(off), name(n), frames() {}
			Scene(uint32_t off, std::string&& n) :
				offset(off), name(n), frames() {}
			Scene(Scene&& old) :
				offset(old.offset), name(std::move(old.name)), frames(std::move(old.frames)) {}
			Scene(const Scene& other) :
				offset(other.offset), name(other.name), frames(other.frames) {}
		};

		FILE* _file;
		Lexer lex;

		std::vector<char> program_data;
		
		int instruction_in_frame;
		int conditional_instructions;
		int choices;

		size_t lineno, columno;

		std::unordered_map<std::string, uint32_t> defined_scenes;
		std::unordered_map<std::string, uint32_t> referenced_scenes;
		std::list<Scene> scenes;

	public:
		Parser() noexcept :
			_file(nullptr), instruction_in_frame(0)
		{}
		Parser(const std::string& filename) :
			_file( [&filename]() {
				FILE* f = fopen(filename.c_str(), "r");
				if (f == nullptr)
					throw std::runtime_error("ERROR: Unable to open file " + filename);
				return f;
			}() ),
			lex(_file),
			instruction_in_frame(0)
		{}

		~Parser() noexcept
		{
			if (_file)
				fclose(_file);
		}

		void open(const std::string& filename);
		void close();

		std::vector<char> parse();

	private:
		void push_int(int value);
		void push_arg(int& lval, Opcode opcode);
		bool try_push_arg(int& lval, Opcode opcode);
		std::string push_and_get_arg(int& lval, Opcode opcode);
		void push_no_arg(int& lval, Opcode opcode);
		void push_var_data(int& lval, Opcode opcode);
	};
}
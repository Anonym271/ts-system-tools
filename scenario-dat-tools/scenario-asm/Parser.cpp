#include "Parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <unordered_map>

#include <tss/KeyTable.hpp>
#include <tss/Opcode.hpp>
#include "lex.yy.h"


namespace tss
{
	void Parser::open(const std::string& filename)
	{
		if (_file)
			fclose(_file);
		_file = fopen(filename.c_str(), "r");
		if (_file == nullptr)
			throw std::runtime_error("Unable to open file " + filename);
		lex = Lexer(_file);
		program_data.clear();
		defined_scenes.clear();
		referenced_scenes.clear();
		instruction_in_frame = 0;
	}

	void Parser::close()
	{
		if (_file)
			fclose(_file);
	}

	void Parser::push_int(int value)
	{
		auto size = program_data.size();
		program_data.resize(program_data.size() + 4);
		int* pos = (int*)(program_data.data() + size);
		*pos = value;
	}

	void Parser::push_arg(int& lval, Opcode opcode)
	{
		if (lval != 's')
			throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
				+ "): Opcode expects an argument: " + opcode_str(opcode));
		lval = lex.lex();

		auto str = lex.string();
		push_int((int)str.size());

		auto size = program_data.size();
		program_data.resize(program_data.size() + str.size());
		char* pos = program_data.data() + size;
		std::memcpy(pos, str.data(), str.size());
	}
	std::string Parser::push_and_get_arg(int& lval, Opcode opcode)
	{
		if (lval != 's')
			throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
				+ "): Opcode expects an argument: " + opcode_str(opcode));
		lval = lex.lex();

		auto str = lex.string();
		push_int((int)str.size());

		auto size = program_data.size();
		program_data.resize(program_data.size() + str.size());
		char* pos = program_data.data() + size;
		std::memcpy(pos, str.data(), str.size());

		return std::string(str.data(), str.size());
	}
	bool Parser::try_push_arg(int& lval, Opcode opcode)
	{
		if (lval == 's')
		{
			push_arg(lval, opcode);
			return 1;
		}
		push_no_arg(lval, opcode);
		return 0;
	}

	void Parser::push_no_arg(int& lval, Opcode opcode)
	{
		if (lval == 's')
			throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
				+ "): Opcode does not accept arguments: " + opcode_str(opcode));
		push_int(0);
	}

	void Parser::push_var_data(int& lval, Opcode opcode)
	{
		if (lval != 'v')
			throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
				+ "): Opcode expects an argument: " + opcode_str(opcode));
		lval = lex.lex();

		auto& data = lex.var_data;
		push_int((int)data.size());
		push_int(0); // has 4 leading 0s for some reason and last string lacks 4 bytes

		auto size = program_data.size();
		program_data.resize(program_data.size() + data.size() - 4);
		char* pos = program_data.data() + size;
		std::memcpy(pos, data.data(), data.size() - 4);
	}


	std::vector<char> Parser::parse()
	{
		instruction_in_frame = -1;
		conditional_instructions = 0;
		choices = 0;
		uint32_t data_offset;
		Opc opcode;
		int lval;
		lval = lex.lex();
		while (lval != '$')
		{
			data_offset = program_data.size();

			lineno = lex.lineno();
			columno = lex.columno();

			if (lval != 'o')
				throw std::runtime_error("ERROR: Invalid token type " + std::to_string(lval)
					+ " at " + std::to_string(lineno) + ':' + std::to_string(lex.columno()));
			opcode = lex.opcode;
			push_int((int)opcode);

			// std::cout << opcode_str(opcode) << '\n';

			if (choices) // is in a choose-block
			{
				if (choices % 2) // second part
				{
					if (opcode != Opc::SELECT)
						throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
							+ "): expected opcode SELECT.");
				}
				else // first part
				{

					if (opcode != Opc::META)
						throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
							+ "): expected opcode META.");
				}
			}

			lval = lex.lex();
			switch (opcode)
			{
			case Opc::SCENE:
			{
				auto scn_name = push_and_get_arg(lval, opcode);
				auto res = defined_scenes.emplace(scn_name, data_offset);
				if (!res.second)
					throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
						+ "): " + scn_name + " is already defined.");

				scenes.emplace_back(data_offset, scn_name);
				if (conditional_instructions)
					throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
						+ "): Unexpected scene change during branch!");
				instruction_in_frame = -1; // start new frame
				break;
			}
			case Opc::JUMP: 
				referenced_scenes.emplace(push_and_get_arg(lval, opcode), (uint32_t)lineno);
				if (!conditional_instructions)
					instruction_in_frame = -1; // start new frame
				break;
			case Opc::CALL:
				referenced_scenes.emplace(push_and_get_arg(lval, opcode), (uint32_t)lineno);
				break;
			case Opc::EXIT:
				// TODO: investigate further
				push_no_arg(lval, opcode);
				break;
			case Opc::SET:
				push_var_data(lval, opcode);
				break;
			case Opc::IF:
				push_var_data(lval, opcode);
				conditional_instructions = 2;
				break;
			case Opc::CHOOSE:
			{
				auto arg = push_and_get_arg(lval, opcode);
				if (arg.size() != 1)
					throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
						+ "): CHOOSE expects exactly one byte as argument!");
				choices = arg[0] * 2 + 1;
				break;
			}
			case Opc::SELECT:
				if (!choices)				
					throw std::runtime_error("ERROR (" + std::to_string(lineno) + ':' + std::to_string(lex.columno())
						+ "): SELECT is only valid in a CHOOSE block!");
				referenced_scenes.emplace(push_and_get_arg(lval, opcode), (uint32_t)lineno);
				break;
			case Opc::RETURN:
				push_no_arg(lval, opcode);
				if (!conditional_instructions)
					instruction_in_frame = -1; // start new frame
				break;
			case Opc::BEGIN:
			case Opc::X_006D:
			case Opc::X_0099:
			case Opc::X_009A:
			case Opc::X_00F7:
			case Opc::X_00F8:
			case Opc::X_0103:
			case Opc::X_0104:
			case Opc::X_0105:
			case Opc::X_0107:
			case Opc::X_0108:
			case Opc::X_0109:
			case Opc::X_010A:
				// don't accept args
				push_no_arg(lval, opcode);
				break;
			case Opc::META:
			case Opc::SPEAKER:
			case Opc::TEXT:
			case Opc::EVENT_CG:
			case Opc::EVENT_CG_:
			case Opc::BG:
			case Opc::X_0069:
			case Opc::X_006B:
			case Opc::X_006C:
			case Opc::X_0075:
			case Opc::SOUND:
			case Opc::X_0078:
			case Opc::CHAR_SPRITE:
			case Opc::CHAR_SPRITE_:
			case Opc::VOICE:
			case Opc::CHAR_MOVE:
			case Opc::VIDEO:
			case Opc::SOUND_:
			case Opc::X_00ED:
			case Opc::X_00EF:
			case Opc::X_00F0:
			case Opc::X_00F1:
			case Opc::CAMERA:
			case Opc::X_00F6:
			case Opc::DATE_A:
			case Opc::DATE_B:
			case Opc::DATE_C:
			case Opc::X_0102:
			case Opc::X_0106:
			case Opc::TITLE:
			case Opc::X_010C:
				// args mandatory
				push_arg(lval, opcode);
				break;
			case Opc::X_0076:
			case Opc::CHAR_HIDE:
			case Opc::X_00BB:
			case Opc::X_00EA:
			case Opc::X_00EB:
			case Opc::X_00EC:
				// args optional
				try_push_arg(lval, opcode);
				break;
			default:
			{
				std::stringstream s("ERROR: Unknown Opcode 0x");
				s << std::hex << std::setw(4) << std::setfill('0') << std::right << (uint32_t)opcode;
				throw std::runtime_error(s.str());
			}
			}
			
			if (instruction_in_frame == 0) // starting new frame
				scenes.back().frames.push_back(data_offset);
			instruction_in_frame++;
			if (conditional_instructions)
				conditional_instructions--;
			if (choices)
				choices--;
		}
	
		// consistency checks
		int undefined_refs = 0;
		for (auto& scn_ref : referenced_scenes)
		{
			if (defined_scenes.find(scn_ref.first) == defined_scenes.end())
			{
				std::cerr << "ERROR: " << scn_ref.first << " (first referenced in line " << scn_ref.second
					<< ") is not defined!\n";
				undefined_refs++;
			}
		}
		if (undefined_refs)
			throw std::runtime_error("ERROR: " + std::to_string(undefined_refs) + " undefined references!");

		// assemble program
		// variables
		size_t offs;
		char* pos;
		std::vector<char> var_section;
		var_section.reserve(lex.variables.size() * 12); // approx. 8 chars + 4 bytes length per variable
		var_section.resize(4);
		*(uint32_t*)var_section.data() = lex.variables.size();
		for (auto& var : lex.variables)
		{
			offs = var_section.size();
			var_section.resize(var_section.size() + 4 + var.size());
			pos = var_section.data() + offs;
			*(uint32_t*)pos = var.size();
			std::memcpy(pos + 4, var.data(), var.size());
		}

		// scenes
		std::vector<char> scn_section;
		scn_section.reserve(scenes.size() * (24 + 4 * 64)); // approx. 64 frames perscene
		scn_section.resize(4);
		*(uint32_t*)scn_section.data() = scenes.size();
		for (auto& scn : scenes)
		{
			offs = scn_section.size();
			scn_section.resize(scn_section.size() + 16 + scn.name.size() + 4 * scn.frames.size());
			pos = scn_section.data() + offs;
			*(uint32_t*)pos = scn.offset;
			pos += 4;
			*(uint32_t*)pos = scn.frames.size();
			pos += 4;
			*(uint32_t*)pos = scn.name.size();
			pos += 4;
			std::memcpy(pos, scn.name.data(), scn.name.size());
			pos += scn.name.size();
			*(uint32_t*)pos = scn.frames.size();
			pos += 4;
			std::memcpy(pos, scn.frames.data(), 4 * scn.frames.size());
		}


		std::vector<char> result(var_section.size() + scn_section.size() + program_data.size() + 4);
		pos = result.data();
		std::memcpy(pos, var_section.data(), var_section.size());
		pos += var_section.size();
		std::memcpy(pos, scn_section.data(), scn_section.size());
		pos += scn_section.size();
		*(uint32_t*)pos = program_data.size();
		pos += 4;
		std::memcpy(pos, program_data.data(), program_data.size());


		std::cout << "Encrypting...\n";
		tss::KeyTable keys;
		keys.decrypt(pos, program_data.size());

		return std::move(result);
	}
}

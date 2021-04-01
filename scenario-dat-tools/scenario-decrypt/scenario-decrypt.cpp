#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <exception>

#include "../include/tss/KeyTable.hpp"

typedef tss::KeyTable keyTable_t;

int main(int argc, const char** argv)
{
	typedef uint32_t uint;

	std::string fn_in, fn_out;
	switch (argc)
	{
	case 3:
		fn_in = argv[1];
		fn_out = argv[2];
		break;
	case 2:
		fn_in = argv[1];
		fn_out = fn_in;
		break;
	default:
		std::cout << "Usage: <me> <input> [output]\n";
		return 0;
	}

	try
	{
		std::ifstream in;
		std::ofstream out;
		in.exceptions(std::ios::badbit | std::ios::failbit);
		out.exceptions(std::ios::badbit | std::ios::failbit);
		in.open(fn_in, std::ios::binary | std::ios::ate);
		auto size = in.tellg();
		in.seekg(0);

		std::vector<char> data(size);
		in.read(data.data(), size);
		in.close();

		char* pos = data.data();
		char* end = pos + data.size();
		uint count, length;

		// skip variable declarations
		if (pos + 4 > end)
			throw std::runtime_error("Unexpected EOF!");
		count = *(uint*)pos;
		pos += 4;
		for (uint i = 0; i < count; i++)
		{
			if (pos + 4 > end)
				throw std::runtime_error("Unexpected EOF!");
			length = *(uint*)pos; 
			pos += 4 + length;
		}

		// skip scene definitions
		if (pos + 4 > end)
			throw std::runtime_error("Unexpected EOF!");
		count = *(uint*)pos;
		pos += 4;
		for (uint i = 0; i < count; i++)
		{
			pos += 8;
			if (pos + 4 > end)
				throw std::runtime_error("Unexpected EOF!");
			length = *(uint*)pos;
			pos += 4 + length;
			if (pos + 4 > end)
				throw std::runtime_error("Unexpected EOF!");
			length = (*(uint*)pos) * 4;
			pos += length + 4;
		}
		
		keyTable_t keys;
		count = *(uint*)pos;
		if (pos + 4 > end)
			throw std::runtime_error("Unexpected EOF!");
		pos += 4;
		if (pos + count > end)
			throw std::runtime_error("Unexpected EOF!");
		keys.decrypt(pos, count);

		out.open(fn_out, std::ios::binary);
		out.write(data.data(), data.size());
	}
	catch (const std::exception& exc)
	{
		std::cout << "Error: " << exc.what() << '\n';
		return 1;
	}
	return 0;
}
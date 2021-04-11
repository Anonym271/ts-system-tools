#include <iostream>

#include "Parser.hpp"

int main(int argc, const char** argv)
{
	std::string fin, fout;
	if (argc < 2 || argc > 3)
	{
		std::cout << "Usage: <me> <input> <output>\n";
		return 0;
	}
	if (argc == 2)
	{
		fin = argv[1];
		fout = "Scenario.dat";
	}
	else
	{
		fin = argv[1];
		fout = argv[2];
	}

	try
	{
		tss::Parser p(fin);
		std::cout << "Compiling...\n";
		auto result = p.parse();
		

		std::ofstream f(fout, std::ios::binary);
		f.exceptions(std::ios::failbit | std::ios::badbit);
		f.write(result.data(), result.size());
		std::cout << "Done.\n";
	}
	catch (const std::ifstream::failure& fexc)
	{
		std::cerr << "ERROR: Failed to write to file " << fout << std::endl;
		return 1;
	}
	catch (const std::exception& exc)
	{
		std::cerr << exc.what() << std::endl;
		return 1;
	}

	return 0;
}
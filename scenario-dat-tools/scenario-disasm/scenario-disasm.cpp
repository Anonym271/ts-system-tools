#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem> 
#include <map>
#include <iomanip>
#include <sstream>

#include <tss/ScenarioDisassembler.hpp>
//#include <tss/ScenarioAnalyzer.hpp>

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: <me> [options] <scenario-file>\nOutput: Disassembled file(s)\n";
        return 0;
    }

    try
    {
        std::string filename(argv[1]);
        
        tss::ScenarioDisassembler disasm(tss::ScenarioDisassembler::UTF8);
      //  disasm.disassemble(filename, ".\\Scenario");
        disasm.disassemble_raw(filename, "scenario_raw.tsa");
    }
    catch (const std::exception& exc)
    {
        std::cerr << "ERROR: " << exc.what() << '\n';
        std::cin.get();
        return 1;
    }
    return 0;
}
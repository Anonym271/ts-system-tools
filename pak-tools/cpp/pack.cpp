#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <list>

#include <cstdint>

namespace fs = std::filesystem;

constexpr int M_EXTRACT = 0;
constexpr int M_REPACK  = 1;
constexpr int MAGIC = 1262698832;

bool do_hse_inversion = true;
bool do_hse_rename = false;

#pragma pack(push, 1)
struct file_table_entry
{
    char name[32];
    uint32_t offset;
    uint32_t length;

    file_table_entry() {}
    file_table_entry(const std::string& filename, uint32_t offs, uint32_t len):
        offset(offs), length(len)
    {
        std::strncpy(name, filename.c_str(), 32);
    }
};

struct dir_table_entry
{
    char name[32];
    uint32_t table_offset;
    uint32_t data_offset;

    dir_table_entry() {}
    dir_table_entry(const std::string& dirname, uint32_t table, uint32_t data):
        table_offset(table), data_offset(data)
    {
        std::strncpy(name, dirname.c_str(), 32);
    }
};
#pragma pack(pop)

void usage_and_exit(int code = 0)
{
    std::cout << 
        "This tool extracts PACK archives (.pak) from the VN Tropical Kiss.\n"
        "Usage: <me> [option] <input> [output]\n"
        "Options:\n"
        "   -e    extract (default)\n"
        "   -p    pack\n"
        "   -h    leave HSE files in raw format (bytes are inverted)\n"
        "   -r    rename .hse files to .png (incompatible with -h, extraction only)\n"
        "           Not recommended since this can't be undone when re-packing.\n"
        "Examples:\n"
        "   <me> chara.pak             > will extract chara.pak to directory 'chara'\n"
        "   <me> -p my_mod_dir mod.pak > will pack contents of dir 'my_mod_dir' into mod.pak\n";
    exit(code);
}

void error_and_exit(const std::string& msg, int code = 1)
{
    std::cerr << "ERROR: " << msg << '\n';
    exit(code);
}

bool iequals(const std::string& s1, const std::string& s2)
{
    if (s1.length() != s2.length())
        return false;
    char c1, c2;
    for (int i = 0; i < s1.length(); i++)
    {
        c1 = s1[i];
        c2 = s2[i];
        if (c1 >= 'A' && c1 <= 'Z')
            c1 -= ('Z' - 'z');
        if (c2 >= 'A' && c2 <= 'Z')
            c2 -= ('Z' - 'z');
        if (c1 != c2)
            return false;
    }
    return true;
}

template<typename T>
T read(std::istream& file)
{
    T t;
    file.read((char*)&t, sizeof(T));
    return t;
}
template<typename T>
T& read(std::istream& file, T& t)
{
    file.read((char*)&t, sizeof(T));
    return t;
}
template<typename T>
void write(std::ostream& file, T& t)
{
    file.write((char*)&t, sizeof(T));
}

void inverse_hse(std::vector<char>& file)
{
    for (int i = 0; i < file.size(); i++)
        file[i] = -file[i];
}

/* EXTRACTION FUNCTIONS */

void extract_file(
    std::istream& file,
    uint32_t offset,
    uint32_t length,
    const fs::path& path)
{
    file.seekg(offset);
    if (file.eof())
        error_and_exit("Unexpected EOF!");

    std::vector<char> data(length);
    file.read(data.data(), length);
    fs::path p(path);
    if (iequals(path.extension().string(), ".hse"))
    {
        if (do_hse_inversion)
            inverse_hse(data);
        if (do_hse_rename)
            p = p.replace_extension(".png");
    }

    std::ofstream fout(p, std::ios::binary);
    if (!fout.good())
        error_and_exit("Could not open file " + p.string());
    fout.write(data.data(), length);
}

void extract_dir(
    std::istream& file, 
    uint32_t table_off, 
    uint32_t data_off, 
    const fs::path& path)
{
    file.seekg(table_off);
    if (file.eof())
        error_and_exit("Unexpected EOF!");

    try
    {
        fs::create_directories(path);
    }
    catch(const std::exception& e)
    {
        error_and_exit("Failed to create directory '" + path.string() + "'");
    }

    uint32_t subdir_count = read<uint32_t>(file);
    std::vector<dir_table_entry> dirs(subdir_count);
    for (uint32_t i = 0; i < subdir_count; i++)
        read(file, dirs[i]);
    uint32_t file_count = read<uint32_t>(file);
    std::vector<file_table_entry> files(file_count);
    for (uint32_t i = 0; i < file_count; i++)
        read(file, files[i]);
  
    for (auto& d : dirs)
    {
        extract_dir(
            file,
            d.table_offset,
            d.data_offset,
            path / d.name
        );
    }

    for (auto& f : files)
    {
        extract_file(
            file,
            f.offset + data_off,
            f.length,
            path / f.name
        );
    }
}

void extract(const std::string& in_file, const std::string& out_dir)
{
    std::ifstream file(in_file, std::ios::binary | std::ios::ate);
    if (!file.good())
        error_and_exit("Failed to open input file!");
    auto fsize = file.tellg();
    file.seekg(0);
    if (fsize < 16)
        error_and_exit("Not a valid PACK file (too small)");
    int magic = read<int>(file);
    if (magic != MAGIC)
        error_and_exit("Nod a valid PACK file: invalid magic numgber");
    uint32_t data_off = read<uint32_t>(file);
    extract_dir(file, 8, data_off, out_dir);
}

/* REPACKING FUNCTIONS */
uint32_t pack_file(std::ostream& file, const fs::path& path)
{
    std::ifstream fin(path, std::ios::binary | std::ios::ate);
    auto len = fin.tellg();
    fin.seekg(0);
    std::vector<char> data(len);
    fin.read(data.data(), len);
    if (iequals(path.extension().string(), ".hse"))
        inverse_hse(data);
    file.write(data.data(), len);
    return len;
}

std::tuple<uint32_t, uint32_t> pack_dir(std::ostream& file, const fs::path& in_dir)
{
    std::list<fs::path> dirs;
    std::list<fs::path> files;
    uint32_t dir_count = 0;
    uint32_t file_count = 0;
    for (auto& e : fs::directory_iterator(in_dir))
    {
        if (e.is_directory())
        {
            dirs.push_back(e.path());
            dir_count++;
        }
        else
        {
            files.push_back(e.path());
            file_count++;
        }
    }

    uint32_t this_table_size = (file_count + dir_count) * 40 + 8;
    uint32_t this_table_offs = file.tellp();
    uint32_t this_data_offs = this_table_offs + this_table_size;

    file.seekp(this_data_offs);
    
    uint32_t length;
    uint32_t offset = 0;
    std::vector<dir_table_entry> dir_entries;
    dir_entries.reserve(dir_count);
    std::vector<file_table_entry> file_entries;
    file_entries.reserve(file_count);


    for (auto& f : files)
    {
        length = pack_file(file, f);
        file_entries.emplace_back(f.filename().string(), offset, length);
        offset += length;
    }
    for (auto& d : dirs)
    {
        auto [sub_table, sub_data] = pack_dir(file, d);
        dir_entries.emplace_back(d.filename().string(), sub_table, sub_data);
    }

    offset = file.tellp();
    file.seekp(this_table_offs);
    write(file, dir_count);
    file.write((char*)dir_entries.data(), dir_count * sizeof(dir_table_entry));
    write(file, file_count);
    file.write((char*)file_entries.data(), file_count * sizeof(file_table_entry));
    file.seekp(offset);

    return {this_table_offs, this_data_offs};
}

void repack(const std::string& in_dir, const std::string& out_file)
{
    std::ofstream file(out_file, std::ios::binary);
    if (!file.good())
        error_and_exit("Could not open file " + out_file);
    write(file, MAGIC);
    file.seekp(8);
    auto [table, data] = pack_dir(file, in_dir);
    file.seekp(4);
    write(file, data);
}


int main(int argc, const char** argv)
{
    // Parse arguments
    if (argc < 2)
        usage_and_exit();

    int mode = M_EXTRACT;
    int arg = 1;
    bool options = true;
    while (arg < argc)
    {
        if (argv[arg][0] != '-')
            break;

        std::string arg_str(argv[arg] + 1);
        if (arg_str == "e")
            mode = M_EXTRACT;
        else if (arg_str == "p")
            mode = M_REPACK;
        else if (arg_str == "h")
            do_hse_inversion = false;
        else if (arg_str == "r")
            do_hse_rename = true;
        else
        {
            std::cerr << "Unknown option '" << argv[arg] << "'!\n\n";
            usage_and_exit(1);
        }
        arg++;
    }

    if (!do_hse_inversion && do_hse_rename)
    {
        std::cerr << "ERROR: options -h and -r are incompatible!\n\n";
        usage_and_exit(1);
    }

    if (arg >= argc)
        usage_and_exit();
    std::string input(argv[arg]);
    std::string output;
    arg++;
    if (arg < argc)
        output = argv[arg++];
    else
    {
        if (mode == M_EXTRACT)
        {
            size_t dotpos = input.find_last_of('.');
            if (dotpos < 0)
            {
                std::cerr << "ERROR: Cannot create output dir since it would be the same as the input file. "
                    "Please offer either a file with extension or an explicit destination.\n\n";
                usage_and_exit(1);
            }
            output = input.substr(0, dotpos);
        }
        else
        {
            output = input + ".pak";
        }
    }

    if (argc > arg)
        std::cout << "WARNING: You have offered too many arguments, the last " << (argc - arg) << " will be ignored.\n";

    // Do stuff
    if (mode == M_EXTRACT)
        extract(input, output);
    else if (mode == M_REPACK)
        repack(input, output);
    else
    {
        std::cerr << "Internal error. What have you done?!\n";
        return 1;
    }
    return 0;
}
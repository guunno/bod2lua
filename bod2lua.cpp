#include <iostream>
#include "dependancy/lz4.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

template <typename T>
void readDecompressed(const char* buffer, size_t& offset, T& outvalue)
{
    std::memcpy(&outvalue, buffer + offset, sizeof(T));
    offset += sizeof(T);
}

template <typename T>
void readDecompressedArray(const char* buffer, size_t& offset, T* dest, size_t len)
{
    std::memcpy(dest, buffer + offset, len);
    offset += len;
    std::cout << offset << "\n";
}

struct float3
{
    float3(float _r = 0, float _g = 0, float _b = 0) : r(_r), g(_g), b(_b) {};
    float r; float g; float b;
};

struct Cell
{
    bool combo;
    uint64_t type; // if combo, type refers to the combo index
    float3 rgb;
    float a;
    int x; int y;
};

std::unordered_map<uint32_t, float3> cellxcolour = {
    { 0x59444F42, float3(0.300, 0.300, 0.300) }, // BODY
    { 0x4D495753, float3(0.500, 0.700, 0.400) }, // SWIM
    { 0x4B454553, float3(0.800, 0.200, 0.900) }, // SEEK
    { 0x44524148, float3(0.900, 0.900, 0.900) }, // HARD
    { 0x50494C53, float3(0.300, 0.400, 0.800) }, // SLIP
    { 0x5447494C, float3(0.900, 0.850, 0.600) }, // LIGT
    { 0x5353414D, float3(0.050, 0.050, 0.050) }, // MASS
    { 0x58454C46, float3(0.800, 0.300, 0.400) }, // FLEX
    { 0x54534C45, float3(0.836, 0.525, 0.255) }, // ELST
    { 0x4E494556, float3(0.162, 0.000, 0.055) }, // VEIN
    { 0x30475254, float3(1.000, 0.200, 0.200) }, // TRG0
    { 0x47525445, float3(1.000, 0.200, 0.600) }, // ETRG
    { 0x4C53554D, float3(0.900, 0.400, 0.400) }, // MUSL
    { 0x4C464E49, float3(0.730, 0.760, 0.170) }, // INFL
    { 0x54524148, float3(0.900, 0.100, 0.100) }, // HART
    { 0x574F4C47, float3(0.900, 0.900, 0.900) }, // GLOW
    { 0x45524957, float3(0.700, 0.700, 0.800) }, // WIRE
    { 0x43504143, float3(0.330, 0.125, 0.003) }, // CAPC
    { 0x43444E49, float3(0.003, 0.125, 0.330) }, // INDC
    { 0x4B4E4953, float3(0.700, 0.700, 0.400) }, // SINK
    { 0x504D4141, float3(0.200, 0.400, 1.000) }, // AAMP
    { 0x5253414C, float3(0.263, 1.000, 0.162) }, // LASR
    { 0x4B495053, float3(0.600, 0.600, 0.600) }, // SPIK
    { 0x524D4148, float3(0.546, 1.000, 0.799) }, // HAMR
    { 0x424D4F42, float3(1.000, 0.300, 0.000) }, // BOMB
    { 0x4B435553, float3(0.400, 1.000, 0.900) }, // SUCK
    { 0x44444152, float3(0.936, 0.744, 0.069) }, // RADD
    { 0x59415453, float3(0.603, 0.909, 1.000) }, // STAY
    { 0x4C454546, float3(0.800, 0.200, 0.400) }, // FEEL
    { 0x584F5250, float3(0.913, 0.084, 0.032) }, // PROX
    { 0x444E5247, float3(0.500, 0.700, 0.700) }, // GRND
    { 0x4E4E4E4F, float3(0.400, 0.300, 0.200) }, // ONNN
    { 0x47524843, float3(0.053, 1.000, 0.986) }, // CHRG
    { 0x54544142, float3(0.039, 0.280, 0.740) }, // BATT
    { 0x5441424E, float3(0.987, 0.568, 0.069) }, // NBAT
    { 0x5445444D, float3(0.700, 0.300, 0.200) }, // MDET
    { 0x4B495453, float3(0.200, 0.600, 0.300) }, // STIK
    { 0x4B4E494C, float3(0.196, 0.029, 1.000) }, // LINK
    { 0x464C4553, float3(0.218, 0.487, 0.133) }, // SELF
    { 0x45544147, float3(0.800, 0.500, 0.500) }, // GATE
    { 0x54454448, float3(0.900, 0.200, 0.200) }, // HDET
    { 0x44455053, float3(0.700, 0.900, 0.200) }, // SPED
    { 0x4D524854, float3(0.300, 0.060, 0.050) }, // THRM
    { 0x5454454A, float3(0.800, 0.800, 0.400) }, // JETT
    { 0x44494341, float3(0.000, 0.600, 0.000) }, // ACID
    { 0x594B4E49, float3(0.010, 0.010, 0.050) }, // INKY
    { 0x4E495053, float3(0.400, 0.900, 0.600) }, // SPIN
    { 0x4843454C, float3(0.166, 0.004, 0.004) }, // LECH
    { 0x434A4E49, float3(0.400, 0.000, 0.100) }, // INJC
    { 0x54535058, float3(0.366, 0.404, 0.604) }, // XPST
    { 0x4D4E4556, float3(0.000, 0.050, 0.000) }, // VENM
    { 0x53494F50, float3(0.300, 0.600, 0.000) }, // POIS
    { 0x59474E41, float3(0.700, 0.150, 0.000) }, // ANGY
    { 0x4F4C4146, float3(0.000, 0.150, 0.700) }, // FALO
    { 0x444E4157, float3(0.182, 0.567, 0.660) }, // WAND
    { 0x4E494E4D, float3(0.700, 0.600, 0.000) }, // MNIN
    { 0x52414546, float3(0.047, 0.313, 0.066) }, // FEAR
    { 0x54494157, float3(0.440, 0.210, 0.560) }, // WAIT
    { 0x444E494D, float3(0.035, 0.000, 0.212) }, // MIND
    { 0x53414850, float3(0.600, 0.700, 1.000) }, // PHAS
    { 0x54414548, float3(0.885, 0.488, 0.307) }, // HEAT
    { 0x47414D45, float3(0.200, 0.050, 0.100) }, // EMAG
    { 0x4C4C4157, float3(0.006, 0.016, 0.050) }, // WALL
    { 0x4147454E, float3(0.900, 0.050, 0.050) }, // NEGA
    { 0x4F54504F, float3(0.104, 0.646, 0.379) }, // OPTO
    { 0x544F4F52, float3(0.211, 0.084, 0.029) }, // ROOT
    { 0x54524F50, float3(0.513, 0.424, 0.405) }, // PORT
    { 0x42415243, float3(0.000, 0.000, 0.000) }, // CRAB
};

std::string extractBodCells(std::vector<Cell>& cells, uint64_t*& combo_cell_list, uint32_t& n_combos)
{
    std::string bodpath;

    std::cout << "Enter .bod filepath\n";
    std::getline(std::cin, bodpath);

    if (!fs::exists(bodpath))
        return "bad path";

    uint32_t bodver;
    uint64_t compressedsize;
    uint64_t uncompressedsize;

    std::ifstream bod(bodpath, std::ios::binary);

    bod.read((char*)&bodver, sizeof(uint32_t));

    if (bodver != 5)
        return "bad bod ver";
    bod.read((char*)&compressedsize, sizeof(uint64_t));
    bod.read((char*)&uncompressedsize, sizeof(uint64_t));

    if (compressedsize > uncompressedsize)
        return "invalid bod (compressed data greater than original data)";

    char* compresseddat = new char[compressedsize];
    char* decompresseddat = new char[uncompressedsize];

    std::cout << "loading " << uncompressedsize << " compressed bytes\n";

    bod.read(compresseddat, compressedsize);

    bod.close();

    std::cout << "Loaded header\n";

    int result = LZ4_decompress_safe(compresseddat, decompresseddat, compressedsize, uncompressedsize);

    if (result >= 0)
        std::cout << "Decompress successful\n";
    else
        return "bad .bod compressed data";

    delete[] compresseddat;

    size_t compressedreadoffset = 0;

    uint32_t half_hex_rotation;
    uint32_t n_normal;
    // n_combos
    uint32_t n_colors;
    uint32_t lx;
    uint32_t ly;
    uint32_t ux;
    uint32_t uy;

    readDecompressed(decompresseddat, compressedreadoffset, half_hex_rotation);
    readDecompressed(decompresseddat, compressedreadoffset, n_normal);
    readDecompressed(decompresseddat, compressedreadoffset, n_combos);
    readDecompressed(decompresseddat, compressedreadoffset, n_colors);
    readDecompressed(decompresseddat, compressedreadoffset, lx);
    readDecompressed(decompresseddat, compressedreadoffset, ly);
    readDecompressed(decompresseddat, compressedreadoffset, ux);
    readDecompressed(decompresseddat, compressedreadoffset, uy);

    int* normal_cell_list = new int[n_normal];
    combo_cell_list = new uint64_t[n_combos];
    float* color_list = new float[n_colors * 4]; // 4 floats (RGBA)

    std::cout << "normal cells: " << n_normal << "\ncombo cells: " << n_combos << "\ncolours: " << n_colors << "\n";

    readDecompressedArray(decompresseddat, compressedreadoffset, normal_cell_list, n_normal * sizeof(int));
    readDecompressedArray(decompresseddat, compressedreadoffset, combo_cell_list, n_combos * sizeof(uint64_t));
    readDecompressedArray(decompresseddat, compressedreadoffset, color_list, n_colors * sizeof(float) * 4);

    uint32_t width = ux - lx;
    uint32_t length = uy - ly;
    uint32_t* cell_array = new uint32_t[width * length];
    uint32_t* color_array = new uint32_t[width * length];

    readDecompressedArray(decompresseddat, compressedreadoffset, cell_array, width * length * sizeof(uint32_t));
    readDecompressedArray(decompresseddat, compressedreadoffset, color_array, width * length * sizeof(uint32_t));

    std::cout << "Loaded compressed data\n";

    cells.reserve(width * length);

    // recursive combos are represented as 4 zero bytes and then the 4 byte index of the combo in combocells

    for (int i = 0; i < width * length; i++)
    {
        uint64_t celltypeidx = cell_array[i];
        if (celltypeidx > 0)
        {
            uint64_t celltype = 0;
            bool combo = false;
            if (celltypeidx > n_normal)
            {
                combo = true;
                celltype = combo_cell_list[celltypeidx - 1 - n_normal];
            }
            else
            {
                celltype = normal_cell_list[celltypeidx - 1];
            }

            if (color_array[i] > 0)
            {
                uint32_t coloridx = color_array[i] - 1;

                cells.push_back(Cell{
                    combo,
                    celltype,
                    float3{
                    color_list[coloridx * 4], color_list[coloridx * 4 + 1], color_list[coloridx * 4 + 2]
                    }, color_list[coloridx * 4 + 3], //rgba
                    int((i % width) + lx), int((i / width) + ly) // x, y and realign

                    }
                );
            }
            else
            {
                float3 c;
                if (!combo)
                    c = cellxcolour.at(uint32_t(celltype));
                cells.push_back(Cell{
                    combo,
                    celltype,
                    c, 1, // add default colour support later
                    int((i % width) + lx), int((i / width) + ly)// x, y and realign

                    }
                );
            }
        }
    }

    return "ok";
}

void passComboCell(std::ofstream& generated, uint64_t* combos, uint32_t comboidx, std::vector<bool>& cleared, std::string& comboarray_name, std::unordered_map<uint32_t, float3>& colours)
{
    // remove combo we're processing from list of not added to list
    cleared[comboidx] = true;

    uint64_t comboname = combos[comboidx];
    uint32_t cella = uint32_t(combos[comboidx]); // lower 32 bits
    uint32_t cellb = uint32_t(combos[comboidx] >> 32); // get upper 32 bits

    float3 coloura;
    float3 colourb;

    std::string line = "";

    if (cella >= 0x80000000) // cella is a combo cell
    {
        uint32_t childcomboidx = cella - 0x80000000;
        if (!cleared[childcomboidx]) // comboa is not already done for us
        {
            // need to recursively go through and add all done combos
            passComboCell(generated, combos, childcomboidx, cleared, comboarray_name, colours);
        }
        line = comboarray_name + "_" + std::to_string(combos[childcomboidx]) + ", ";
        coloura = colours.at(combos[childcomboidx]);
    }
    else
    {
        char* cellname = (char*)&cella;
        line += "\"";
        line.append(cellname, 4);
        line += "\", ";

        coloura = cellxcolour.at(cella);
    }

    if (cellb >= 0x80000000) // cellb is a combo cell
    {
        uint32_t childcomboidx = cellb - 0x80000000;
        if (!cleared[childcomboidx]) // combob is not already done for us
        {
            // need to recursively go through and add all done combos
            passComboCell(generated, combos, childcomboidx, cleared, comboarray_name, colours);
        }
        line += comboarray_name + "_" + std::to_string(combos[childcomboidx]);
        colourb = colours.at(combos[childcomboidx]);
    }
    else
    {
        char* cellname = (char*)&cellb;
        line += "\"";
        line.append(cellname, 4);
        line += "\"";

        colourb = cellxcolour.at(cellb);
    }

    float3 average = { (coloura.r + colourb.r) / 2.f, (coloura.g + colourb.g) / 2.f, (coloura.b + colourb.b) / 2.f };
    colours.emplace(comboname, average);

    generated << comboarray_name << "_" << std::to_string(comboname) << " = get_combo_id( " << line << " )\n";
}

std::string passComboCells(std::ofstream& generated, uint64_t* combos, uint32_t n_combos, std::unordered_map<uint32_t, float3>& colours)
{
    if (n_combos > 0)
    {
        generated << "-- Add combos\n\n";

        std::string comboarrayname;
        std::cout << "Enter combo cells name: ";
        std::getline(std::cin, comboarrayname);

        std::vector<bool> cleared;
        for (int i = 0; i < n_combos; i++) cleared.push_back(false);

        for (int i = 0; i < n_combos; i++)
        {
            if (!cleared[i])
                passComboCell(generated, combos, i, cleared, comboarrayname, colours);
        }

        return comboarrayname;
    }
    return "";
}

std::string outputLua(std::vector<Cell>& cells, uint64_t* combos, uint32_t n_combos)
{
    std::string filename;
    std::string bodid;
    std::cout << "Enter output file name: ";
    std::getline(std::cin, filename);
    std::cout << "Enter body_id var name: ";
    std::getline(std::cin, bodid);

    std::ofstream generated(filename);

    if (!generated)
        return "bad file";

    std::unordered_map<uint32_t, float3> combocellcolours;
    std::string comboname = passComboCells(generated, combos, n_combos, combocellcolours);

    generated << "\n-- Add cells\n\n";

    std::cout << cells.size() << " cells found\n";

    for (int i = 0; i < cells.size(); i++)
    {
        // set_plan_cell(body_id, q, r, type, r, g, b, a)
        char* cellname = (char*)&cells[i].type;

        if (cells[i].combo)
        {
            generated << "set_plan_cell( "
                << bodid // body_id
                << ", " << std::to_string(cells[i].x) << ", " << std::to_string(cells[i].y) // q, r (game stores it as qy no conversion needed)
                << ", " << comboname << "_" << cells[i].type // if normal then the type is just 4 chars wearing a uint32 as a trenchcoat
                << ", " << std::to_string(combocellcolours.at(cells[i].type).r) << ", " << std::to_string(combocellcolours.at(cells[i].type).g)
                << ", " << std::to_string(combocellcolours.at(cells[i].type).b) << ", " << std::to_string(cells[i].a) // colour
                << " )\n"; // close func and new bracket
        }
        else
        {
            generated << "set_plan_cell( "
                << bodid // body_id
                << ", " << std::to_string(cells[i].x) << ", " << std::to_string(cells[i].y) // q, r (game stores it as qy no conversion needed)
                << ", \"" << cellname[0] << cellname[1] << cellname[2] << cellname[3] << "\"" // if normal then the type is just 4 chars wearing a uint32 as a trenchcoat
                << ", " << std::to_string(cells[i].rgb.r) << ", " << std::to_string(cells[i].rgb.g) << ", " << std::to_string(cells[i].rgb.b) << ", " << std::to_string(cells[i].a) // colour
                << " )\n"; // close func and new bracket
        }
    }
    
    generated.close();

    return "ok";
}

int main()
{
    std::cout << "Gunno's Bod2Lua \nusing .bod format 5 and LZ4 compression version " << LZ4_versionString() << "\n";

    std::vector<Cell> cells;

    uint64_t* combos = nullptr;
    uint32_t numcombos = 0;

    std::string err = extractBodCells(cells, combos, numcombos);
    std::cout << "Extracted bod cells: " << err << "\n";

    if (err != "ok")
    {
        std::cout << "Cannot continue\n";
        std::system("pause");
        return -1;
    }

    err = outputLua(cells, combos, numcombos);
    std::cout << "Created lua: " << err << "\n";

    delete[] combos;
    
    if (err != "ok")
    {
        std::cout << "Cannot continue\n";
        std::system("pause");
        return -1;
    }

    std::cout << "\n\nDone!\n";

    std::system("pause");
}

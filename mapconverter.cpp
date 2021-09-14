
#include "load_save_png.hpp"
#include "read_write_chunk.hpp"
#include "PPU466.hpp"

#include <fstream>
#include <iostream>

const int tl_w = 8;
const int tl_h = 8;
const int tl_table_w = 1;
const int tl_table_h = 1;
//const int pl_num = 8;
//const int cl_num = 4;

void SetTilePixel(PPU466::Tile &tile, int x, int y, uint8_t color_idx)
{
    uint8_t bit0 = color_idx & 1;
    uint8_t bit1 = (color_idx >> 1) & 1;
    tile.bit0[y] &= (~(1 << x));
    tile.bit0[y] |= (bit0 << x);

    tile.bit1[y] &= (~(1 << x));
    tile.bit1[y] |= (bit1 << x);
}
uint8_t GetTilePixel(const PPU466::Tile &tile, int x, int y)
{
    uint8_t bit0 = (tile.bit0[y] >> x) & 1;
    uint8_t bit1 = (tile.bit1[y] >> x) & 1;

    return (bit1 << 1) | bit0;
}
void print_tile(const PPU466::Tile &tile)
{
    printf("printing tile ING.....:\n");
    for (int y = tl_h - 1; y >= 0; y--)
    {
        for (int x = 0; x < tl_w; x++)
        {
            std::cout << GetTilePixel(tile, x, y) << ' ';
        }
        std::cout << std::endl;
    }
}
void print_cl(glm::u8vec4 cl)
{
    std::cout << (unsigned int)cl.x << ' ' << (unsigned int)cl.y << ' ' << (unsigned int)cl.z << ' ' << (unsigned int)cl.w <<std::endl;
}
void print_palette(const PPU466::Palette &pl)
{
    for (int i = 0; i < 4; i++)
    {
        print_cl(pl[i]);
    }
}
void evolve(glm::u8vec4 &cl)
{
    if (cl.x > 246 && cl.y > 246 && cl.z > 246)
    {
        cl.w = 0;
    }
    else
        cl.w = 255;
}
std::string level_path(std::string const &filename, std::string const &suffix)
{
    return "../assets/" + filename + "." + suffix;
}
std::string png_str;
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <png file name without suffix>" << std::endl;
        return -1;
    }
    //read png name

    png_str = argv[1];

    std::cout << "Handling png_name ... :" << png_str << std::endl;

    glm::uvec2 size;
    std::vector<glm::u8vec4> png_data;
    load_png(level_path(png_str, "png"), &size, &png_data, UpperLeftOrigin);
    std::ofstream tl_output(level_path(png_str, "tl"), std::ios::binary);
    std::ofstream pl_output(level_path(png_str, "pl"), std::ios::binary);

    std::vector<PPU466::Palette> pl_table;
    std::vector<PPU466::Tile> tl_table(tl_table_w * tl_table_h, PPU466::Tile());

    printf("png tile width: %d height: %d\n", size.x, size.y);
        glm::u8vec4 eem{0, 0, 0, 0};
        PPU466::Palette em{eem, eem, eem, eem};
        PPU466::Palette pl = em;
        int cnt = 0;
        for (int i = 0; i < (int)size.x; i++)
        {
            for (int j = 0; j < (int)size.y; j++)
            {
                int k = i * size.y + j;
                auto cl = png_data[k];
                evolve(cl);
                if (std::find(pl.begin(), pl.end(), cl) == pl.end())
                {
                    pl[cnt] = cl;
                    cnt++;
                    if (cnt == 4)
                    {
                        pl_table.push_back(std::move(pl));
                        pl = em;
                    }
                }
                //print_cl(pl[j]);
            }
        }
        if (cnt < 4)
        {
            pl_table.push_back(std::move(pl));
            pl = em;
        }
    for (int i = 0; i < tl_table_h; i++)
    {
        for (int j = 0; j < tl_table_w; j++)
        {
            //if (i || j)

            {
                auto &tl = tl_table[i * tl_table_w + j];
                std::vector<glm::u8vec4> prep;
                for (int row = i * tl_h; row < (i + 1) * tl_h; row++)
                {
                    for (int col = j * tl_w; col < (j + 1) * tl_w; col++)
                    {
                        auto cl = png_data[row * tl_w * tl_table_w + col];
                        evolve(cl);
                        if (std::find(prep.begin(), prep.end(), cl) == prep.end())
                        {
                            prep.push_back(cl);

                            if (prep.size() > 4)
                            {
                                std::cerr << "More than 4 colors in " << i << ' ' << j << std::endl;
                                return -1;
                            }
                        }
                    }
                }
                std::cout << "tttttttttttttttt\n";
                for (auto &cl : prep)
                {
                    print_cl(cl);
                }
                std::cout << "tttttttttttttttt\n";
                for (int k = 0; k < 4; k++)
                {
                    print_palette(pl_table[k]);
                    std::cout << "ffffffffffffffffffffff" << std::endl;
                }

                int pl_idx = -1;
                size_t pl_num = pl_table.size();
                for (size_t i = 0; i < pl_num; i++)
                {
                    auto &pl = pl_table[i];
                    bool pl_match = true;
                    for (auto &cl : prep)
                    {
                        if (std::find(pl.begin(), pl.end(), cl) == pl.end())
                        {
                            pl_match = false;
                            break;
                        }
                    }
                    if (pl_match)
                    {
                        pl_idx = static_cast<int>(i);
                        break;
                    }
                }
                if (pl_idx < 0)
                {
                    printf("fuck %d, %d\n", i, j);
                    return -1;
                }
                auto &pl = pl_table[pl_idx];
                std::cout << "fuck1\n";
                for (int x = 0; x < tl_w; x++)
                {
                    for (int y = 0; y < tl_h; y++)
                    {
                        int row = i * tl_h + (7 - y);
                        int col = j * tl_w + x;
                        auto &cl = png_data[row * tl_w * tl_table_w + col];
                        uint8_t cl_idx = static_cast<uint8_t>(std::distance(pl.begin(), std::find(pl.begin(), pl.end(), cl)));
                        SetTilePixel(tl, x, y, cl_idx);
                    }
                }
                std::cout << "fuck2\n";
            }
        }
    }
    size_t x = png_str.size();
    pl_table.resize(1);
    print_palette(pl_table[0]);

    write_chunk((png_str.substr(x-3, 3) + "p").c_str(), pl_table, &pl_output);
    write_chunk((png_str.substr(x-3, 3) + "t").c_str(), tl_table, &tl_output);

    return 0;
}
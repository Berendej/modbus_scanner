#include <string>
#include <cstdint>

#include "defs.h"

namespace mb_scanner
{

std::string to_hex(std::uint8_t uc)
{
    std::string res;
    static const char tab[16] = { '0', '1', '2', '3', '4', '5', '6', '7', 
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::uint8_t h = uc >> 4;
    res.push_back(tab[h]);
    std::uint8_t l = uc & 0xF;
    res.push_back(tab[l]);
    return res;
} 


std::string to_hex(const buffer_c &ar, const std::size_t length)
{
    std::string res; 
    std::size_t ix = 0;
    while ( ix < length )
    {
        res += to_hex(ar[ix]);
        res += " ";
        ix++;
    }
    return res;
}

} // namespace mb_scanner
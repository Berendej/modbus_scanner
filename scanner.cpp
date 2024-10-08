#include "endians.h"
#include <cstdint>

namespace mb_scanner
{

union aux_two_bytes_u
{
    std::uint8_t m_bytes[2];
    std::uint16_t m_ui16;
};

std::uint16_t combine(std::uint8_t hi, std::uint8_t lo)
{
    aux_two_bytes_u res;
    if (endian_c::little)
    {
        res.m_bytes[0] = lo;
        res.m_bytes[1] = hi;
    }
    else
    {
        res.m_bytes[0] = hi;
        res.m_bytes[1] = lo;
    }
    return res.m_ui16;
}

bool valid_fc(std::uint8_t f)
{
    switch(f)
    {
    case 1: case 2: case 3: case 4:  case 5:
    case 6: case 15: case 16: case 23:
        return true;
    }
    return false;
}

} // namespace mb_scanner
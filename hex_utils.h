#pragma once

#include <string>

namespace mb_scanner
{

std::string to_hex(const buffer_c &ar, const std::size_t length);
std::string to_hex(std::uint8_t uc);
std::string to_hex(const uchar_vect_t& uv);

}

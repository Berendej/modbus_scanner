#pragma once

#include <cstdint>

namespace mb_scanner
{
/**
 * \brief Determine big/little platform endianess.
 *
 * Endians constexpr'es big/little help to choose iteration direction
 * while scanning integral types byte by byte.
 */
class endian_c
{
private:
    static constexpr std::uint32_t uint32_ = 0x01020304;
    static constexpr std::uint8_t magic_ = (const std::uint8_t&)uint32_;
public:
    static constexpr bool little = magic_ == 0x04;
    static constexpr bool big = magic_ == 0x01;
};

}// namespace mb_scanner
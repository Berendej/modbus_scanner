#pragma once 

namespace mb_scanner
{

bool check_crc_rtu(const uchar_vect_t &buf,
                  std::uint8_t received_crc_hi,
                  std::uint8_t received_crc_lo );

} // namespace mb_scanner

#pragma once

#include "defs.h"

namespace mb_scanner
{
    void* create_scanner(protocol_e prot, 
                        int address, 
                        pdu_callback_t pdu_cb,
                        error_callback_t err_cb,
                        nonsense_callback_t nonse_cb,
                        void *user);

    void delete_scanner(void *scanner);
    bool scan(void *scanner, const buffer_c *p_input );
    std::string to_hex(const buffer_c &ar, const std::size_t length);
    std::string to_hex(const uchar_vect_t& uv);
    std::string to_hex(std::uint8_t uc);

    std::uint16_t combine(std::uint8_t hi, std::uint8_t lo);
    void get_hi_lo(uint16_t reg, uint8_t &hi, uint8_t &lo);
    void calc_crc_rtu(const buffer_c& buf, std::uint8_t &crc_hi, std::uint8_t &crc_lo);

}


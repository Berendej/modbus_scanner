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

}


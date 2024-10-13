#pragma once 

#include "scanner.h"

namespace mb_scanner
{

    class slave_rtu_scanner_c : public scanner_c
    {
        co_char_t::pull_type m_coro;
        void  scan_co(co_char_t::push_type& sink);

    public:
       slave_rtu_scanner_c(int address, 
                      pdu_callback_t pdu_cb,
                      error_callback_t err_cb,
                      nonsense_callback_t nonse_cb,
                      void *user);
        virtual void scan(const buffer_c *p_input);
 
    };

}

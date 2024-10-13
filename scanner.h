#pragma once

#include "defs.h"
#include <boost/coroutine2/all.hpp>

namespace mb_scanner
{

    typedef boost::coroutines2::coroutine<const buffer_c* >  co_char_t;

    class scanner_c
    {
    protected:
        pdu_callback_t m_pdu_cb;
        error_callback_t m_err_cb;
        nonsense_callback_t m_nonse_cb;
        std::uint8_t m_address;
        void *m_user;
        pdu_c m_pdu;

        const buffer_c *m_p_input;
        std::size_t m_ix;

    public:
        scanner_c(int address, 
                  pdu_callback_t pdu_cb,
                  error_callback_t err_cb,
                  nonsense_callback_t nonse_cb,
                  void *user) :
                m_address(address)
                , m_user(user)
                , m_pdu_cb(pdu_cb)
                , m_err_cb(err_cb)
                , m_nonse_cb(nonse_cb)
        {
        }

        virtual void scan(const buffer_c *p_input) = 0;
    };

    bool valid_fc(std::uint8_t f);
    std::uint16_t combine(std::uint8_t hi, std::uint8_t lo);

}
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "slave_tcp_scanner.h"
#include "endians.h"
#include "hex_utils.h"

//#define VERBOSE 1

#ifdef VERBOSE
#include <iostream>
#endif

namespace mb_scanner
{
    slave_tcp_scanner_c::slave_tcp_scanner_c(int address, 
                      pdu_callback_t pdu_cb,
                      error_callback_t err_cb,
                      nonsense_callback_t nonse_cb,
                      void *user) 
                : scanner_c(address, pdu_cb, err_cb, nonse_cb, user)
                , m_coro(( [&](co_char_t::push_type& sink) { scan_co(sink); }))
    {

    }

    void slave_tcp_scanner_c::scan(const buffer_c *p_input)
    {
        // passing params to coro
        m_p_input = p_input;
        #ifdef VERBOSE
        std::cout << "scan chunk size " << (int)m_p_input->actual_size() << std::endl;
        #endif
        m_ix = 0;
        m_coro();
    }

    void slave_tcp_scanner_c::scan_co(co_char_t::push_type& sink)
    {
        #define check_buf_end  m_ix++; if (m_ix == m_p_input->actual_size()) sink(m_p_input)

        int ix;
        int length = 0;
        uint8_t length_1, length_2;
        int fc = 0;
        uint8_t i16_1_hi = 0;
        uint8_t i16_1_lo = 0;
        uint8_t i16_2_hi = 0;
        uint8_t i16_2_lo = 0;
        uint8_t i16_3_hi = 0;
        uint8_t i16_3_lo = 0;
        uint8_t i16_4_hi = 0;
        uint8_t i16_4_lo = 0;
        // initially at first call 
        // jump over initial state
        #ifdef VERBOSE
        std::cout << "first call" << std::endl;
        #endif
        goto pre_transact_state;
    initial_state:
        #ifdef VERBOSE
        std::cout << "initial state" << std::endl;
        #endif
    pre_transact_state: 
        sink(m_p_input);
    transact_id_state:
        m_pdu.m_transact_id_1 = (*m_p_input)[m_ix];
        check_buf_end;
        m_pdu.m_transact_id_2 = (*m_p_input)[m_ix];
        check_buf_end;
    protocol_id_state:
        // m_pdu.m_protocol_id = (*m_p_input)[m_ix]; // no need
        check_buf_end;
        check_buf_end; // two meaningless bytes

    length_state:
        length_1 = (*m_p_input)[m_ix];
        check_buf_end;
        length_2 = (*m_p_input)[m_ix];
        length = combine(length_1, length_2);
#ifdef VERBOSE
        std::cout << "length = " << length << std::endl;
#endif
        check_buf_end;

    //state unit_id_state:
        if ( (*m_p_input)[m_ix] != m_address)
        {
            // not our address
            // must drop the rest of current buffer
#ifdef VERBOSE
            std::cout << "wrong unit id " << (int)(*m_p_input)[m_ix]
                      << std::endl;
            std::cout << to_hex( *m_p_input, m_p_input->actual_size()) 
                << std::endl;
#endif
            goto pre_transact_state ;
        }
        // got our address
        #ifdef VERBOSE
        std::cout << "got unit id " << (int)(*m_p_input)[m_ix] << std::endl;
        #endif
        check_buf_end;
    // state : function 
        if ( valid_fc( (*m_p_input)[m_ix] ) )
        {
            fc = (*m_p_input)[m_ix];
        }
        else
        {    //invalid fc, nonsense input
            #ifdef VERBOSE
            std::cout << "invalid fc " << (int)(*m_p_input)[m_ix] << std::endl;
            #endif
            // must drop the rest of current buffer
            m_nonse_cb(m_user);
             goto pre_transact_state;
        }
        #ifdef VERBOSE
        std::cout << "fc " << fc << std::endl;
        #endif
        check_buf_end;
    // state first reg addr hi
        i16_1_hi = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "got i16_1_hi " << std::endl;
        #endif
        check_buf_end;
    // state first reg addr low
        i16_1_lo = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "got i16_1_lo " << std::endl;
        #endif
        check_buf_end;
        // depending on fc following state
    // state num reg hi
        i16_2_hi = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "got i16_2_hi " << (int)i16_2_hi << std::endl;
        #endif
        check_buf_end;
    // state num reg low
        i16_2_lo = (*m_p_input)[m_ix] ;
        #ifdef VERBOSE
        std::cout << "got i16_1_lo " << (int)i16_2_lo << std::endl;
        #endif
        switch( fc )
        {
        case 23:
            check_buf_end;
            i16_3_hi = (*m_p_input)[m_ix];
            check_buf_end;
            i16_3_lo = (*m_p_input)[m_ix];
            check_buf_end;
            i16_4_hi = (*m_p_input)[m_ix];
            check_buf_end;
            i16_4_lo = (*m_p_input)[m_ix];
            m_pdu.m_i16_3 = combine(i16_3_hi, i16_3_lo);
            m_pdu.m_i16_4 = combine(i16_4_hi, i16_4_lo);
            // do not break! fall though
        case 15: case 16:
            check_buf_end;
            m_pdu.m_data.clear();
            m_pdu.m_i8_1 = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << " i8_1 " << (int)m_pdu.m_i8_1 << std::endl;
            #endif
            check_buf_end;
            ix = 0;
            while( ix < m_pdu.m_i8_1 )
            {
                #ifdef VERBOSE
                std::cout << to_hex( (*m_p_input)[m_ix] ) << " ";
                #endif
                m_pdu.m_data.push_back( (*m_p_input)[m_ix] );
                ix++;
                if ( ix < m_pdu.m_i8_1)
                {
                    check_buf_end;
                }
            } // end of fc15 special case
            #ifdef VERBOSE
            std::cout << std::endl;
            #endif
            break;
        } 
        #ifdef VERBOSE
        std::cout << "got whole pdu" << std::endl;
        #endif
    whole_pdu:
        m_pdu.m_i16_1 = combine(i16_1_hi, i16_1_lo);
        m_pdu.m_i16_2 = combine(i16_2_hi, i16_2_lo);
        m_pdu.m_fc = fc;
        m_pdu_cb(m_user, m_pdu);
        check_buf_end;
        goto transact_id_state;
    } // scan_co

} // namespace

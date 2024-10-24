#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "slave_rtu_scanner.h"
#include "crc_check.h"
#include "endians.h"
#include "hex_utils.h"

//#define VERBOSE 1

#ifdef VERBOSE
#include <iostream>
#endif

namespace mb_scanner
{

    slave_rtu_scanner_c::slave_rtu_scanner_c(int address, 
                      pdu_callback_t pdu_cb,
                      error_callback_t err_cb,
                      nonsense_callback_t nonse_cb,
                      void *user) 
                : scanner_c(address, pdu_cb, err_cb, nonse_cb, user)
                , m_coro(( [&](co_char_t::push_type& sink) { scan_co(sink); }))
    {

    }

    void slave_rtu_scanner_c::scan(const buffer_c *p_input)
    {
        // passing params to coro
        m_p_input = p_input;
        #ifdef VERBOSE
        std::cout << "scan chunk size " << m_p_input->actual_size() << std::endl;
        #endif
        m_ix = 0;
        m_coro();
    }

    void slave_rtu_scanner_c::scan_co(co_char_t::push_type& sink)
    {
        #define check_buf_end  m_ix++; if (m_ix == m_p_input->actual_size()) sink(m_p_input)

        int ix;
        int length = 0;
        int fc = 0;
        uint8_t i16_1_hi = 0;
        uint8_t i16_1_lo = 0;
        uint8_t i16_2_hi = 0;
        uint8_t i16_2_lo = 0;
        uint8_t i16_3_hi = 0;
        uint8_t i16_3_lo = 0;
        uint8_t i16_4_hi = 0;
        uint8_t i16_4_lo = 0;
        uchar_vect_t crc_buf;
        uint8_t crc_hi = 0;
        uint8_t crc_low = 0;
        // initially at first call skip initial state
        crc_buf.reserve(256);
        goto pre_address_state;
    initial_state:
        crc_buf.clear();
        #ifdef VERBOSE
        std::cout << "initial state" << std::endl;
        #endif
    pre_address_state: 
        sink(m_p_input);
    address_state:
        while ( m_ix != m_p_input->actual_size() && 
                m_address != (*m_p_input)[m_ix] )
        {
            m_ix++;
        }
        if (m_ix == m_p_input->actual_size() )
        {
            // not our address and the whole chunk scanned
            goto initial_state;
        }
        // got our address
        #ifdef VERBOSE
        std::cout << "address ok" << std::endl;
        #endif
        crc_buf.push_back((*m_p_input)[m_ix]);
        check_buf_end;
    // state : function 
        if ( valid_fc( (*m_p_input)[m_ix] ) )
        {
            crc_buf.push_back( (*m_p_input)[m_ix] );
            fc = (*m_p_input)[m_ix];
        }
        else
        {    //invalid fc, nonsense input
            #ifdef VERBOSE
            std::cout << "invalid fc " << (int)(*m_p_input)[m_ix] << std::endl;
            #endif
            m_nonse_cb(m_user);
            goto initial_state;
        }
        check_buf_end;
    // state i16_1 hi        
        crc_buf.push_back( (*m_p_input)[m_ix] );
        i16_1_hi = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "i16_1_hi " << (int )i16_1_hi << std::endl;
        #endif
        check_buf_end;
    // state i16_1 low
        crc_buf.push_back( (*m_p_input)[m_ix] );
        i16_1_lo = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "i16_1_lo " << (int )i16_1_lo << std::endl;
        #endif
        check_buf_end;
    // state i16_2 hi
        crc_buf.push_back((*m_p_input)[m_ix]);
        i16_2_hi = (*m_p_input)[m_ix];
        #ifdef VERBOSE
        std::cout << "i16_2_hi " << (int )i16_2_hi << std::endl;
        #endif
        check_buf_end;
    // state i16_2 lo
        crc_buf.push_back( (*m_p_input)[m_ix] );
        i16_2_lo = (*m_p_input)[m_ix] ;
        #ifdef VERBOSE
        std::cout << "i16_2_lo " << (int )i16_2_lo << std::endl;
        #endif
        check_buf_end;
        switch( fc )
        {
        case 1: case 2: case 3: case 4: case 5: case 6:
            break;
        case 23:
            // state i16_3 hi
            crc_buf.push_back( (*m_p_input)[m_ix] );
            i16_3_hi = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << "i16_3_hi " << (int )i16_3_hi << std::endl;
            #endif
            check_buf_end;
            // state i16_3 lo
            crc_buf.push_back( (*m_p_input)[m_ix] );
            i16_3_lo = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << "i16_3_lo " << (int )i16_3_lo << std::endl;
            #endif
            check_buf_end;
            // state i16_4 hi
            crc_buf.push_back( (*m_p_input)[m_ix] );
            i16_4_hi = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << "i16_4_hi " << (int )i16_4_hi << std::endl;
            #endif
            check_buf_end;
            // state i16_4 lo
            crc_buf.push_back( (*m_p_input)[m_ix] );
            i16_4_lo = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << "i16_4_lo " << (int )i16_4_lo << std::endl;
            #endif
            m_pdu.m_i16_3 = combine(i16_3_hi, i16_3_lo);
            m_pdu.m_i16_4 = combine(i16_4_hi, i16_4_lo);
            check_buf_end;
            // do not break! fall though
        case 15: case 16:
            m_pdu.m_data.clear();
            // state i8_1
            crc_buf.push_back( (*m_p_input)[m_ix] );
            m_pdu.m_i8_1 = (*m_p_input)[m_ix];
            #ifdef VERBOSE
            std::cout << "i8_1 " << (int )m_pdu.m_i8_1 << std::endl;
            #endif
            check_buf_end;
            ix = 0;
            // state data
            #ifdef VERBOSE
            std::cout << "enter data state " <<  std::endl;
            #endif
            while( ix < m_pdu.m_i8_1 )
            {
                crc_buf.push_back( (*m_p_input)[m_ix] );
                m_pdu.m_data.push_back( (*m_p_input)[m_ix] );
                check_buf_end;
                ix++;
            }
            #ifdef VERBOSE
            std::cout << "leave data state " <<  std::endl;
            #endif
        }
    // state crc hi
        crc_hi = (*m_p_input)[m_ix];
        check_buf_end;
    // state crc low
        crc_low = (*m_p_input)[m_ix];
        if (check_crc_rtu(crc_buf, crc_hi, crc_low))
        {
            #ifdef VERBOSE
            std::cout << "pdu ok " << std::endl;
            #endif
            m_pdu.m_i16_1 = combine(i16_1_hi, i16_1_lo);
            m_pdu.m_i16_2 = combine(i16_2_hi, i16_2_lo);
            m_pdu.m_fc = fc;
            m_pdu_cb(m_user, m_pdu);
            check_buf_end;
        }
        else // crc is wrong
        {
            #ifdef VERBOSE
            std::cout << "invalid crc " << to_hex(crc_buf) << std::endl;
            #endif
            m_nonse_cb(m_user);
            goto initial_state;
        }
        crc_buf.clear();
        goto address_state;
    } // scan_co

} // namespace

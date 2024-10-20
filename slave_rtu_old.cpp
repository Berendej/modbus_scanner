#include "slave_rtu_old.h"
//#define VERBOSE 1

#ifdef VERBOSE
#include <iostream>
#endif

namespace mb_scanner
{

extern bool check_crc_rtu(const uchar_vect_t &buf,
                  std::uint8_t received_crc_hi,
                  std::uint8_t received_crc_lo );
extern bool valid_fc( std::uint8_t c );

slave_rtu_old_c::slave_rtu_old_c(int address, 
                      pdu_callback_t pdu_cb,
                      error_callback_t err_cb,
                      nonsense_callback_t nonse_cb,
                      void *user) 
                : scanner_c(address, pdu_cb, err_cb, nonse_cb, user)
    , m_calculated_crc_hi(0)
    , m_calculated_crc_lo(0)
    , m_fc(0)
    , m_i16_1_hi(0)
    , m_i16_1_lo(0)
    , m_i16_2_hi(0)
    , m_i16_2_lo(0)
    , m_i16_3_hi(0)
    , m_i16_3_lo(0)
    , m_i16_4_hi(0)
    , m_i16_4_lo(0)
    , m_i8_1(0)
    , m_crc1(0)
    , m_crc2(0)
    , m_sub_diag1(0)
    , m_sub_diag2(0)
    , m_diag_data1(0)
    , m_diag_data2(0)
    , m_init_state(this)
    , m_fc_state(this)
    , m_i16_1_hi_state(this)
    , m_i16_1_lo_state(this)
    , m_i16_2_hi_state(this)
    , m_i16_2_lo_state(this)
    , m_i16_3_hi_state(this)
    , m_i16_3_lo_state(this)
    , m_i16_4_hi_state(this)
    , m_i16_4_lo_state(this)
    , m_i8_1_state(this)
    , m_data_state(this)
    , m_sub_diag_1_state(this)
    , m_sub_diag_2_state(this)
    , m_diag_data_1_state(this)
    , m_diag_data_2_state(this)
    , m_cs_1_state(this)
    , m_cs_2_state(this)
{
    m_p_current_state = &m_init_state;
}

void slave_rtu_old_c::scan(const buffer_c *p_input)
{
    std::uint16_t ix = 0;
    if ( p_input == nullptr )
    {
        return;
    }
    while ( ix < p_input->actual_size() )
    {
        m_p_current_state->on_char(((*p_input)[ix]));
        ix++;
    }
}

void slave_rtu_old_c::put_in_buffer(std::uint8_t c)
{
    m_crc_buffer.push_back(c);
    // for crc calculation
}

void slave_rtu_old_c::switch_state(state_enum se)
{
    switch(se)
    {
    case init_state_e:
        m_crc_buffer.clear();
#ifdef VERBOSE
        log_msg("switch to init");
#endif
        m_p_current_state = &m_init_state;
        break;
    case fc_state_e:
#ifdef VERBOSE
        log_msg("switch to fc");
#endif
        m_p_current_state = &m_fc_state;
        break;
    case i16_1_hi_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_1_hi");
#endif
        m_p_current_state = &m_i16_1_hi_state;
        break;
    case i16_1_lo_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_1_lo");
#endif
        m_p_current_state = &m_i16_1_lo_state;
        break;
    case i16_2_hi_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_2_hi");
#endif
        m_p_current_state = &m_i16_2_hi_state;
        break;
    case i16_2_lo_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_2_lo");
#endif
        m_p_current_state = &m_i16_2_lo_state;
        break;
    case i16_3_hi_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_3_hi_state");
#endif
        m_p_current_state = &m_i16_3_hi_state;
        break;

    case i16_3_lo_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_3_lo_state");
#endif
        m_p_current_state = &m_i16_3_lo_state;
        break;

    case i16_4_hi_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_4_hi_state");
#endif
        m_p_current_state = &m_i16_4_hi_state;
        break;

    case i16_4_lo_state_e:
#ifdef VERBOSE
        log_msg("switch to i16_4_lo_state");
#endif
        m_p_current_state = &m_i16_4_lo_state;
        break;

    case i8_1_state_e:
#ifdef VERBOSE
        log_msg("switch to i8_1_state");
#endif
        m_p_current_state = &m_i8_1_state;
        break;

    case data_state_e:
#ifdef VERBOSE
        log_msg("switch to data_state");
#endif
        m_p_current_state = &m_data_state;
        break;

        // diagnostics
    case sub_diag_1_state_e:
        log_msg("switch to sub diag1");
        m_p_current_state = &m_sub_diag_1_state;
        break;

    case sub_diag_2_state_e:
        log_msg("switch to sub diag2");
        m_p_current_state = &m_sub_diag_2_state;
        break;

    case diag_data_1_state_e:
        log_msg("switch to diag data 1");
        m_p_current_state = &m_diag_data_1_state;
        break;

    case diag_data_2_state_e:
        log_msg("switch to diag data 2");
        m_p_current_state = &m_diag_data_2_state;
        break;

    case checksum_1_state_e:
#ifdef VERBOSE
        log_msg("switch to crc1");
#endif
        m_p_current_state = &m_cs_1_state;
        break;
    case checksum_2_state_e:
#ifdef VERBOSE
        log_msg("switch to crc2");
#endif
        m_p_current_state = &m_cs_2_state;
        break;
    }
}

// *******************************************************
//          state methods
// *******************************************************
void init_state::on_char(std::uint8_t c)
{
    if ( c != m_p_owner->m_address )
    {
        //continue to stay in init_state
        return;
    }
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(fc_state_e);
}

void fc_state::on_char(std::uint8_t c)
{
    if ( valid_fc(c) )
    {
        m_p_owner->m_fc = c;
        switch( c )
        {
        case 8:
            m_p_owner->switch_state(sub_diag_1_state_e);
            break;
        default:
            m_p_owner->switch_state(i16_1_hi_state_e);
            break;
        }
        m_p_owner->put_in_buffer(c);
    }
    else
    {
        // should wait
        m_p_owner->switch_state(init_state_e);
    }
}

void i16_1_hi_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_1_hi = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_1_lo_state_e);
}

void i16_1_lo_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_1_lo = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_2_hi_state_e);
}

void i16_2_hi_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_2_hi = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_2_lo_state_e);
}

void i16_2_lo_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_2_lo = c;
    m_p_owner->put_in_buffer(c);
    switch( m_p_owner->m_fc )
    {
    case 15: case 16:
        m_p_owner->switch_state(i8_1_state_e);
        break;
    case 23:
        m_p_owner->switch_state(i16_3_hi_state_e);
        break;
    default: // case 1: case 2: case 3: case 4: case 5: case 6:
        m_p_owner->switch_state(checksum_1_state_e);
        break;
    }
}

void checksum_1_state::on_char(std::uint8_t c)
{
    m_p_owner->m_crc1 = c;
    m_p_owner->switch_state(checksum_2_state_e);
}

void checksum_2_state::on_char(std::uint8_t c)
{
    m_p_owner->m_crc2 = c;
    m_p_owner->process_pdu();
}

void slave_rtu_old_c::log_msg(const char* msg)
{
    #ifdef VERBOSE
    std::cout << msg << std::endl;
    #endif
}

void i16_3_hi_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_3_hi = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_3_lo_state_e);
}

void i16_3_lo_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_3_lo = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_4_hi_state_e);
}

void i16_4_hi_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_4_hi = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i16_4_lo_state_e);
}

void i16_4_lo_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i16_4_lo = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(i8_1_state_e);
}

void i8_1_state::on_char(std::uint8_t c)
{
    m_p_owner->m_i8_1 = c;
    m_p_owner->put_in_buffer(c);
    m_p_owner->clear_value();
    m_p_owner->switch_state(data_state_e);
}

void slave_rtu_old_c::clear_value()
{
    m_pdu.m_data.clear();
}

void slave_rtu_old_c::put_value_char(std::uint8_t c)
{
    m_pdu.m_data.push_back(c);
    put_in_buffer(c);
}

void data_state::on_char(std::uint8_t c)
{
    if ( m_index < m_p_owner->m_i8_1 )
    {
        m_p_owner->put_value_char(c);
        m_index++;
    }
    // we must switch to checksum_1_state upon receiving the
    // last value symbol
    if ( m_index == m_p_owner->m_i8_1 )
    {
        m_index = 0;
        m_p_owner->switch_state(checksum_1_state_e);
    }
}

// diagnostic subfunc start
void sub_diag_1_rtu_s_state::on_char(std::uint8_t c)
{
    m_p_owner->set_sub_diag1(c);
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(sub_diag_2_state_e);
}

void sub_diag_2_rtu_s_state::on_char(std::uint8_t c)
{
    m_p_owner->set_sub_diag2(c);
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(diag_data_1_state_e);
}

void diag_data_1_rtu_s_state::on_char(std::uint8_t c)
{
    m_p_owner->set_diag_data1(c);
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(diag_data_2_state_e);
}

void diag_data_2_rtu_s_state::on_char(std::uint8_t c)
{
    m_p_owner->set_diag_data2(c);
    m_p_owner->put_in_buffer(c);
    m_p_owner->switch_state(checksum_1_state_e);
}

void slave_rtu_old_c::process_pdu()
{
    // check if crc is valid:
    if( !check_crc_rtu(m_crc_buffer, m_crc1, m_crc2 ))
    {
        // response with an error
        log_msg("crc is wrong");
        /*
            If the server receives the request, but detects
            a communication error (parity, LRC, CRC, ...),
            no response is returned. The client program will
            eventually process a timeout condition for the
            request.
        */
        m_nonse_cb(m_user);
    }
    else
    {
        // all right - response with a data
        m_pdu.m_fc = m_fc;
        m_pdu.m_i16_1 = combine(m_i16_1_hi, m_i16_1_lo);
        m_pdu.m_i16_2 = combine(m_i16_2_hi, m_i16_2_lo);
        m_pdu.m_i8_1 = m_i8_1;
        m_pdu.m_fc = m_fc;
        m_pdu.m_i16_3 = combine(m_i16_3_hi, m_i16_3_lo);
        m_pdu.m_i16_4 = combine(m_i16_4_hi, m_i16_4_lo);
        log_msg("crc is good");
        m_pdu_cb(m_user, m_pdu);
    }
    switch_state(init_state_e);
}

} // namespace mb_scanner
#pragma once

#include "scanner.h"

namespace mb_scanner
{

// forward decl
class slave_rtu_old_c; 

class scan_state
{
protected:
    slave_rtu_old_c * m_p_owner;
public:
    virtual void on_char(std::uint8_t c) = 0;
};

class init_state : public scan_state
{
public:
    init_state(slave_rtu_old_c* p_scanner)  {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class fc_state : public scan_state
{
public:
    fc_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

// diagnostic subfunc start
class sub_diag_1_rtu_s_state : public scan_state
{
public:
    sub_diag_1_rtu_s_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class sub_diag_2_rtu_s_state : public scan_state
{
public:
    sub_diag_2_rtu_s_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class diag_data_1_rtu_s_state : public scan_state
{
public:
    diag_data_1_rtu_s_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class diag_data_2_rtu_s_state : public scan_state
{
public:
    diag_data_2_rtu_s_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

// diagnostic subfunc end

class i16_1_hi_state : public scan_state
{
public:
    i16_1_hi_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_1_lo_state : public scan_state
{
public:
    i16_1_lo_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_2_hi_state : public scan_state
{
public:
    i16_2_hi_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_2_lo_state : public scan_state
{
public:
    i16_2_lo_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_3_hi_state : public scan_state
{
public:
    i16_3_hi_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_3_lo_state : public scan_state
{
public:
    i16_3_lo_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_4_hi_state : public scan_state
{
public:
    i16_4_hi_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i16_4_lo_state : public scan_state
{
public:
    i16_4_lo_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class i8_1_state : public scan_state
{
public:
    i8_1_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class data_state : public scan_state
{
public:
    unsigned char m_index;
    data_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; m_index = 0; }
    virtual void on_char(std::uint8_t c);
};

class checksum_1_state : public scan_state
{
public:
    checksum_1_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};

class checksum_2_state : public scan_state
{
public:
    checksum_2_state(slave_rtu_old_c* p_scanner) {  m_p_owner = p_scanner; }
    virtual void on_char(std::uint8_t c);
};


enum state_enum
{
    init_state_e,
    fc_state_e,
    i16_1_hi_state_e,
    i16_1_lo_state_e,
    i16_2_hi_state_e,
    i16_2_lo_state_e,

    i16_3_hi_state_e,
    i16_3_lo_state_e,
    i16_4_hi_state_e,
    i16_4_lo_state_e,
    i8_1_state_e,
    data_state_e,

    // diagnostics
    sub_diag_1_state_e,
    sub_diag_2_state_e,
    diag_data_1_state_e,
    diag_data_2_state_e,
    checksum_1_state_e,
    checksum_2_state_e
};

    class slave_rtu_old_c : public scanner_c
    {
    private:
        // borrow start
        friend class init_state; 
        friend class fc_state; 
        friend class i16_1_hi_state;
        friend class i16_1_lo_state;
        friend class i16_2_hi_state;
        friend class i16_2_lo_state;

        friend class i16_3_hi_state;
        friend class i16_3_lo_state;
        friend class i16_4_hi_state;
        friend class i16_4_lo_state;
        friend class i8_1_state;
        friend class data_state;

        friend class sub_diag_1_rtu_s_state;
        friend class sub_diag_2_rtu_s_state;
        friend class diag_data_1_rtu_s_state;
        friend class diag_data_2_rtu_s_state;

        friend class checksum_1_state;
        friend class checksum_2_state;

        unsigned char m_calculated_crc_hi;
        unsigned char m_calculated_crc_lo;
        void log_msg(const char* msg);
        std::uint8_t m_fc;

        std::uint8_t m_i16_1_hi;
        std::uint8_t m_i16_1_lo;
        std::uint8_t m_i16_2_hi;
        std::uint8_t m_i16_2_lo;
        std::uint8_t m_i16_3_hi;
        std::uint8_t m_i16_3_lo;
        std::uint8_t m_i16_4_hi;
        std::uint8_t m_i16_4_lo;
        std::uint8_t m_i8_1; // m_byte_count;
        std::uint8_t m_crc1;
        std::uint8_t m_crc2;
        uchar_vect_t m_crc_buffer;

        std::uint8_t m_sub_diag1;
        std::uint8_t m_sub_diag2;
        std::uint8_t m_diag_data1;
        std::uint8_t m_diag_data2;

        // parse states:
        init_state            m_init_state;
        fc_state              m_fc_state;
        i16_1_hi_state        m_i16_1_hi_state;
        i16_1_lo_state        m_i16_1_lo_state;
        i16_2_hi_state        m_i16_2_hi_state;
        i16_2_lo_state        m_i16_2_lo_state;

        i16_3_hi_state        m_i16_3_hi_state;
        i16_3_lo_state        m_i16_3_lo_state;
        i16_4_hi_state        m_i16_4_hi_state;
        i16_4_lo_state        m_i16_4_lo_state;
        i8_1_state            m_i8_1_state;
        data_state            m_data_state;

        sub_diag_1_rtu_s_state      m_sub_diag_1_state;
        sub_diag_2_rtu_s_state      m_sub_diag_2_state;
        diag_data_1_rtu_s_state     m_diag_data_1_state;
        diag_data_2_rtu_s_state     m_diag_data_2_state;

        checksum_1_state m_cs_1_state;
        checksum_2_state m_cs_2_state;
        // end of parse states
        scan_state *m_p_current_state;
        void switch_state(state_enum se);
        void process_pdu();
        void clear_value();
        void put_value_char(std::uint8_t c);
        void put_in_buffer(std::uint8_t c); // for crc calculation
        // diagnostic starts
        void set_sub_diag1(std::uint8_t c) { m_sub_diag1 = c; }
        void set_sub_diag2(std::uint8_t c) { m_sub_diag2 = c; }
        void set_diag_data1(std::uint8_t c) { m_diag_data1 = c; }
        void set_diag_data2(std::uint8_t c) { m_diag_data2 = c; }
        // diagnostic ends
    public:
       slave_rtu_old_c(int address, 
                      pdu_callback_t pdu_cb,
                      error_callback_t err_cb,
                      nonsense_callback_t nonse_cb,
                      void *user);
        virtual void scan(const buffer_c *p_input);
    };

}

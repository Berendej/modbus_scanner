#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace mb_scanner
{
    using uchar_vect_t = std::vector<std::uint8_t>;
    using i_uchar_vect_t = uchar_vect_t::iterator;
    using ic_uchar_vect_t = uchar_vect_t::const_iterator;

    enum  class protocol_e
    {
        master_rtu,
        master_ascii,
        master_tcp,
        slave_rtu,
        slave_ascii,
        slave_tcp
    };

constexpr uint16_t BUF_SIZE = 1024;

    struct buffer_c : public std::array<uint8_t, BUF_SIZE>
    {
        buffer_c() : m_actual_size(0)
        {

        }

        buffer_c( std::initializer_list<uint8_t> il ) : m_actual_size(il.size())
        {
            int ix = 0;
            for( auto u8 : il )
            {
                (*this)[ix++] = u8;
            }
        }

        std::size_t actual_size() const
        {
            return m_actual_size;
        }

        void set_actual_size(std::size_t sz)
        {
            m_actual_size = sz;
        }

    private:
        std::size_t m_actual_size;
    };

    struct pdu_c
    {
        std::uint8_t m_transact_id_1; // for tcp only
        std::uint8_t m_transact_id_2;

        std::uint8_t m_fc;
        std::uint16_t m_i16_1;
        std::uint16_t m_i16_2;
        std::uint16_t m_i16_3;
        std::uint16_t m_i16_4;
        std::uint8_t  m_i8_1;
        std::vector<std::uint8_t> m_data;

        #define F1_REQ_STARTING_ADDRESS  m_i16_1
        #define F1_REQ_QUANTITY_OF_COILS m_i16_2
        #define F1_RESP_BYTE_COUNT  m_i8_1

        #define F2_REQ_STARTING_ADDRESS  m_i16_1
        #define F2_REQ_QUANTITY_OF_INPUTS m_i16_2
        #define F2_RESP_BYTE_COUNT  m_i8_1

        #define F3_REQ_STARTING_ADDRESS  m_i16_1
        #define F3_REQ_QUANTITY_OF_REGS m_i16_2
        #define F3_RESP_BYTE_COUNT  m_i8_1

        #define F4_REQ_STARTING_ADDRESS  m_i16_1
        #define F4_REQ_QUANTITY_OF_REGS m_i16_2
        #define F4_RESP_BYTE_COUNT  m_i8_1

        #define F5_REQ_OUTPUT_ADDRESS  m_i16_1
        #define F5_REQ_OUTPUT_VALUE    m_i16_2

        #define F5_RESP_OUTPUT_ADDRESS  m_i16_1
        #define F5_RESP_OUTPUT_VALUE    m_i16_2

        #define F6_REQ_REG_ADDRESS  m_i16_1
        #define F6_REQ_REG_VALUE    m_i16_2

        #define F6_RESP_REG_ADDRESS  m_i16_1
        #define F6_RESP_REG_VALUE    m_i16_2

        #define F15_REQ_STARTING_ADDRESS m_i16_1
        #define F15_REQ_QUANTITY_OF_OUTPUTS m_i16_2
        #define F15_REQ_BYTE_COUNT  m_i8_1

        #define F15_RESP_STARTING_ADDRESS m_i16_1
        #define F15_RESP_QUANTITY_OF_OUTPUTS m_i16_2

        #define F16_REQ_STARTING_ADDRESS m_i16_1
        #define F16_REQ_QUANTITY_OF_REGS m_i16_2
        #define F16_REQ_BYTE_COUNT  m_i8_1

        #define F16_RESP_STARTING_ADDRESS m_i16_1
        #define F16_RESP_QUANTITY_OF_REGS m_i16_2

        #define F23_REQ_READ_STARTING_ADDRESS m_i16_1
        #define F23_REQ_QUANTITY_TO_READ m_i16_2
        #define F23_REQ_WRITE_STARTING_ADDRESS m_i16_3
        #define F23_REQ_QUANTITY_TO_WRITE m_i16_4
        #define F23_REQ_WRITE_BYTE_COUNT  m_i8_1

        #define F23_RESP_BYTE_COUNT  m_i8_1

    };

    typedef void (*pdu_callback_t) (void*, const pdu_c& );
    typedef void (*error_callback_t) (void*, int );
    typedef void (*nonsense_callback_t) (void* ); 


} // namespace mb_scanner
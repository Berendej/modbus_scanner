#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "server_connection.h"

//#define VERBOSE 1



server_connection_c::server_connection_c(boost::asio::ip::tcp::socket sock) : 
    m_socket{std::move(sock)}
{
    m_p_modbus_scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_tcp, 
        255, // address 255
        on_pdu_aux,
        on_error_aux,
        on_nonse_aux,
        this);

#ifdef VERBOSE
    std::cout << "server_connection_c " << std::endl;
#endif
}

void server_connection_c::start_read()
{
#ifdef VERBOSE
    std::cout << "start read " << m_socket.is_open() << std::endl;
#endif

    m_socket.async_read_some(
        boost::asio::buffer(m_buffer.data(), m_buffer.max_size()),
        boost::bind(&server_connection_c::handle_read, 
                     shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));
}

void server_connection_c::handle_read(const boost::system::error_code error,
                  const std::size_t length) 
{
#ifdef VERBOSE
    std::cout << "handle read " << m_socket.is_open() << std::endl;
    std::cout << mb_scanner::to_hex(m_buffer, length) << std::endl;
#endif
    if (error or 0 == length ) 
    {
#ifdef VERBOSE
        std::cout << "handle read error " << error << " << len " << length << std::endl;
#endif
        // zero length also means connection is closed
        // connection is closed, we do not continue 
        // async calling so that smart pointer will do its work
        // and connection_c will be deleted
        return;
    }
#ifdef VERBOSE
    std::cout << "handle read ok" << std::endl;
#endif
    if ( m_p_modbus_scanner )
    {
        m_buffer.set_actual_size(length);
        mb_scanner::scan(m_p_modbus_scanner, &m_buffer );
    }
    start_read();
}


server_connection_c::~server_connection_c()
{
#ifdef VERBOSE
    std::cout << "~server_connection_c " << m_socket.is_open() << std::endl;
#endif
}

// scanner part

void server_connection_c::on_pdu(const mb_scanner::pdu_c& pdu)
{
        std::cout << "server_connection_c::on_pdu f " << (int)pdu.m_fc;
        switch( pdu.m_fc)
        {
        case 1:
            std::cout << "\n F1_STARTING_ADDRESS  " << (int)pdu.F1_REQ_STARTING_ADDRESS
                      << "\n F1_QUANTITY_OF_COILS " << (int)pdu.F1_REQ_QUANTITY_OF_COILS 
                      << std::endl;
            break;
        case 2:
            std::cout << "\n F2_REQ_STARTING_ADDRESS  " << (int)pdu.F2_REQ_STARTING_ADDRESS
                      << "\n F2_REQ_QUANTITY_OF_INPUTS " << (int)pdu.F2_REQ_QUANTITY_OF_INPUTS 
                      << std::endl;
            break;
        case 3:
            std::cout << "\n F3_REQ_STARTING_ADDRESS  " << (int)pdu.F3_REQ_STARTING_ADDRESS
                      << "\n F3_REQ_QUANTITY_OF_REGS  " << (int)pdu.F3_REQ_QUANTITY_OF_REGS
                      << std::endl;

            break;
        case 4:
            std::cout << "\n F4_REQ_STARTING_ADDRESS " <<  (int)pdu.F4_REQ_STARTING_ADDRESS
                      << "\n F4_REQ_QUANTITY_OF_REGS " << (int)pdu.F4_REQ_QUANTITY_OF_REGS
                      << std::endl;
            break;
        case 5:
            std::cout << "\n F5_REQ_OUTPUT_ADDRESS " <<  (int)pdu.F5_REQ_OUTPUT_ADDRESS
                      << "\n F5_REQ_OUTPUT_VALUE " << (int)pdu.F5_REQ_OUTPUT_VALUE
                      << std::endl;
            break;
        case 6:
            std::cout << "\n F6_REQ_REG_ADDRESS " << (int)pdu.F6_REQ_REG_ADDRESS
                      << "\n F6_REQ_REG_VALUE " << (int)pdu.F6_REQ_REG_VALUE
                      << std::endl;
            break;
        case 15:
            std::cout <<  "\n F15_REQ_STARTING_ADDRESS " << (int)pdu.F15_REQ_STARTING_ADDRESS
                      <<  "\n F15_REQ_QUANTITY_OF_OUTPUTS " << (int)pdu.F15_REQ_QUANTITY_OF_OUTPUTS
                      <<  "\n F15_REQ_BYTE_COUNT " << (int)pdu.F15_REQ_BYTE_COUNT
                      << std::endl;
            for(auto uc : pdu.m_data )
            { 
                std::cout << (int)uc << " ";
            }
            std::cout << std::endl;
            break;
        case 16:
            std::cout << "\n F16_REQ_STARTING_ADDRESS " << (int)pdu.F16_REQ_STARTING_ADDRESS
                      << "\n F16_REQ_QUANTITY_OF_REGS " << (int)pdu.F16_REQ_QUANTITY_OF_REGS
                      << "\n F16_REQ_BYTE_COUNT " << (int)pdu.F16_REQ_BYTE_COUNT
                      << std::endl;
            for(auto uc : pdu.m_data )
            { 
                std::cout << (int)uc << " ";
            }
            std::cout << std::endl;
            break;
        case 23:
            std::cout << "\n F23_REQ_READ_STARTING_ADDRESS " << (int)pdu.F23_REQ_READ_STARTING_ADDRESS
                      << "\n F23_REQ_QUANTITY_TO_READ " << (int)pdu.F23_REQ_QUANTITY_TO_READ
                      << "\n F23_REQ_WRITE_STARTING_ADDRESS " << (int)pdu.F23_REQ_WRITE_STARTING_ADDRESS
                      << "\n F23_REQ_QUANTITY_TO_WRITE " << (int)pdu.F23_REQ_QUANTITY_TO_WRITE
                      << "\n F23_REQ_WRITE_BYTE_COUNT " << (int)pdu.F23_REQ_WRITE_BYTE_COUNT
                      << std::endl;
            for(auto uc : pdu.m_data )
            { 
                std::cout << (int)uc << " ";
            }
            std::cout << std::endl;
            break;
        }
}

void server_connection_c::on_error(int err)
{
    std::cout << "user::on_err " << err << std::endl;
}

void server_connection_c::on_nonse()
{
    std::cout << "user::on_nonsense " << std::endl;
}

// static
void server_connection_c::on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu)
{
    server_connection_c *p_this{(server_connection_c*)obj};
    p_this->on_pdu(pdu);
}

// static
void server_connection_c::on_error_aux(void* obj, int err)
{
    server_connection_c *p_this{(server_connection_c*)obj};
    p_this->on_error(err);
}

// static
void server_connection_c::on_nonse_aux(void* obj)
{
    server_connection_c *p_this{(server_connection_c*)obj};
    p_this->on_nonse();
}

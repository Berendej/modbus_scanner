#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "rtu_server_data_handler.h"

#define VERBOSE 1

rtu_server_data_handler_c::rtu_server_data_handler_c(boost::asio::io_service &io_service,
        std::string port_name,
        uint8_t dev_address) : 
    m_serial_port(io_service)
    , m_port_name(port_name)
    , m_sec_timer(io_service, std::chrono::seconds{1})
    , m_dev_address(dev_address)
{
    m_p_modbus_scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_rtu_old, 
        m_dev_address, // address 255
        on_pdu_aux,
        on_error_aux,
        on_nonse_aux,
        this);

#ifdef VERBOSE
    std::cout << "rtu_server_data_handler_c " << std::endl;
#endif
}

rtu_server_data_handler_c::~rtu_server_data_handler_c()
{
    m_serial_port.close();
    m_sec_timer.cancel();
}

bool rtu_server_data_handler_c::start()
{
    if ( !open_port() )
    {
        std::cout << "open_port failed \n";
        return false;
    }
    std::cout << "open port success" << std::endl;
    schedule_timer();
    start_read();
    return true;
}

void rtu_server_data_handler_c::schedule_timer()
{
    m_sec_timer.expires_after( std::chrono::seconds{3} );
    m_sec_timer.async_wait(
        boost::bind(  &rtu_server_data_handler_c::handle_timer, 
                      shared_from_this(),
                      boost::asio::placeholders::error));
}

void rtu_server_data_handler_c::handle_timer(const boost::system::error_code& error)
{
    if ( error )
    {
        return;
    }
    //std::cout << "tick\n";
    schedule_timer();
}

bool rtu_server_data_handler_c::set_port_config(int fd)
{
    termios opt;
    if (tcgetattr(fd, &opt) == -1) 
    {
        std::cout << "set_port_config: " << strerror(errno)
                  << std::endl;
        return false;
    }
    // Raw mdoe
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // Set speed
    cfsetispeed(&opt, 19200);
    cfsetospeed(&opt, 19200);
    opt.c_cflag &= ~HUPCL;
    opt.c_cflag |= (CLOCAL | CREAD);
    // Set data bits count
    opt.c_cflag &= ~CSIZE; /* Mask the character size bits */
    opt.c_cflag |= CS8;
    // Set parity
    // PARITY_NONE:
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~PARODD;
    if (tcsetattr(fd, TCSANOW, &opt) == -1) 
    {
        std::cout << "tcsetattr failed: " << strerror(errno);
        return false;
    }
    return true;
}

bool rtu_server_data_handler_c::open_port()
{
    int fd = open(m_port_name.c_str(), O_NONBLOCK | O_RDWR, S_IRWXU);
    if ( -1 == fd ) 
    {
        std::cout << "Failed to open port " << m_port_name.c_str() 
          << strerror(errno) << std::endl;
        return false;
    }
    if (!isatty(fd)) 
    {
        std::cout << "Serial port " << m_port_name 
                  << " not a tty device" << std::endl;
        close(fd);
        return false;
    }

    if ( !set_port_config(fd) )
    {
        close(fd);
        return false;
    }
    tcdrain(fd);
    boost::system::error_code ec;
    m_serial_port.assign(fd, ec);
    return true;
}

void rtu_server_data_handler_c::start_read()
{
    m_serial_port.async_read_some(
            boost::asio::buffer(m_input_buffer.data(), m_input_buffer.max_size()),
            boost::bind(&rtu_server_data_handler_c::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void rtu_server_data_handler_c::handle_read(const boost::system::error_code error,
                  const std::size_t length) 
{
#ifdef VERBOSE
    std::cout << mb_scanner::to_hex(m_input_buffer, length) << std::endl;
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
        m_input_buffer.set_actual_size(length);
        mb_scanner::scan(m_p_modbus_scanner, &m_input_buffer );
    }
}

void rtu_server_data_handler_c::reply_header(std::uint8_t fc)
{
    m_output_buffer.set_actual_size(0);
    m_output_buffer.add(m_dev_address);
    m_output_buffer.add(fc);
}

bool rtu_server_data_handler_c::get_coil(std::uint16_t index )
{
    if ( m_coils.end() == m_coils.find(index) )
    {
        // new one insert with initial false value
        m_coils[index] = false;
        return false;
    }
    return (m_coils[index]);
}

void rtu_server_data_handler_c::set_coil(std::uint16_t index, std::uint16_t val )
{
    #ifdef VERBOSE
    std::cout << "set coil " << index << " = " << val << std::endl;
    #endif
    m_coils[index] = ( 0 == val ) ? false : true;
}

void rtu_server_data_handler_c::set_reg(std::uint16_t index, std::uint16_t val )
{
    #ifdef VERBOSE
    std::cout << "set reg " << index << " = " << val << std::endl;
    #endif
    m_regs[index] = val;
}

uint16_t rtu_server_data_handler_c::get_reg(std::uint16_t index)
{
    if ( m_regs.end() == m_regs.find(index) )
    {
        // new one insert with initial false value
        m_regs[index] = 0;
        return 0;
    }
    return (m_regs[index]);
}

void rtu_server_data_handler_c::reply_f1(std::uint8_t fc,
                                         std::uint16_t f1_req_starting_address, 
                                         std::uint16_t f1_req_quantity_of_coils)
{
    #ifdef  VERBOSE
    std::cout << "reply_f1 start" << std::endl;
    #endif
    reply_header(fc);
    uint8_t byte_count;
    byte_count = f1_req_quantity_of_coils/8;
    if ( f1_req_quantity_of_coils % 8 > 0 )
    {
        byte_count++;
    }
    m_output_buffer.add(byte_count);
    uint8_t byte = 0;
    uint8_t aux_byte = 0;
    int bit_ix = 0;
    std::uint16_t ix = f1_req_starting_address;
    uint16_t quant = 0;
    while(quant < f1_req_quantity_of_coils)
    {
        aux_byte = get_coil(ix) ? 1 : 0;
        if(aux_byte)
        {
            aux_byte = aux_byte << bit_ix;
            byte |= aux_byte;
        }
        bit_ix++;
        if ( 8 == bit_ix )
        {
            m_output_buffer.add(byte);
            byte = 0;
            bit_ix = 0;
        }
        ix++;
        quant++;
    }
    // possibly last byte left 
    if (bit_ix > 0)
    {
        m_output_buffer.add(byte);
    }
    add_crc();
    send_pdu();
    #ifdef  VERBOSE
    std::cout << "reply_f1 finish" << std::endl;
    #endif
}

void rtu_server_data_handler_c::reply_f2(std::uint8_t fc,
                    std::uint16_t f2_req_starting_address,
                    std::uint16_t f2_req_quantity_of_inputs)
{
    reply_f1(fc, f2_req_starting_address, f2_req_quantity_of_inputs);
}

void rtu_server_data_handler_c::reply_f3(std::uint8_t fc,
                                         std::uint16_t f3_req_starting_address, 
                                         std::uint16_t f3_req_quantity_of_regs)
{
    reply_header(fc);
    uint16_t reg = 0;
    uint8_t hi, lo;
    std::uint16_t ix = f3_req_starting_address;
    std::uint16_t quant = 0;
    uint8_t byte_count = f3_req_quantity_of_regs * 2;
    m_output_buffer.add(byte_count);
    while(quant < f3_req_quantity_of_regs)
    {
        reg = get_reg(ix);
        mb_scanner::get_hi_lo(reg, hi, lo);
        m_output_buffer.add(hi);
        m_output_buffer.add(lo);
        ix++;
        quant++;
    }
    add_crc();
    send_pdu();
}

void rtu_server_data_handler_c::reply_f4(std::uint8_t fc,
                                    std::uint16_t f4_req_starting_address,
                                    std::uint16_t f4_req_quantity_of_regs)
{
    reply_f3(4, f4_req_starting_address, f4_req_quantity_of_regs);
}

void rtu_server_data_handler_c::reply_f5(std::uint8_t fc,
                  std::uint16_t f5_req_output_address,
                  std::uint16_t f5_req_output_value)
{
    set_coil(f5_req_output_address, f5_req_output_value);
    mirror_pdu();
}

void rtu_server_data_handler_c::reply_f6(std::uint8_t fc,
                  std::uint16_t f6_req_reg_address,
                  std::uint16_t f6_req_reg_value)
{
    set_reg(f6_req_reg_address, f6_req_reg_value);
    mirror_pdu();
}

void rtu_server_data_handler_c::reply_f15(std::uint8_t fc,
                  std::uint16_t f15_req_starting_address,
                  std::uint16_t f15_req_quantity_of_outputs,
                  std::uint8_t f15_req_byte_count,
                  const mb_scanner::uchar_vect_t &data )
{
    reply_header(fc);
    std::uint8_t aux_byte;
    std::uint8_t byte_val;
    std::uint8_t byte_ix = 0;
    std::uint8_t hi, lo;
    std::uint16_t current_coil_address = f15_req_starting_address;
    std::uint16_t coil_count = 0;
    std::uint8_t bit_mask = 0;
    std::uint8_t bit_cnt = 0;
    while(byte_ix < f15_req_byte_count &&
          coil_count < f15_req_quantity_of_outputs )
    {
        byte_val = data[byte_ix];
        #ifdef VERBOSE
        std::cout << "f15 set byte " << mb_scanner::to_hex(byte_val) << std::endl;
        #endif 
        bit_mask = 1;
        bit_cnt = 0;
        while( bit_cnt < 8  && coil_count < f15_req_quantity_of_outputs )
        {
            aux_byte = byte_val & bit_mask;
            if ( 0 != aux_byte )
            {
                set_coil(current_coil_address, true);
            }
            else
            {
                set_coil(current_coil_address, false);
            }
            current_coil_address++;
            coil_count++;
            bit_mask = bit_mask << 1; // 1 2 4 .. 0x80
            bit_cnt++;
        }
        byte_ix++;
    }
    mb_scanner::get_hi_lo(f15_req_starting_address, hi, lo);
    m_output_buffer.add(hi);
    m_output_buffer.add(lo);
    mb_scanner::get_hi_lo(f15_req_quantity_of_outputs, hi, lo);
    m_output_buffer.add(hi);
    m_output_buffer.add(lo);
    add_crc();
    send_pdu();
}

void rtu_server_data_handler_c::reply_f16(std::uint8_t fc,
                   std::uint16_t f16_req_starting_address,
                   std::uint16_t f16_req_quantity_of_regs,
                   std::uint8_t f16_req_byte_count,
                   const mb_scanner::uchar_vect_t &data)
{
    reply_header(fc);
    std::uint8_t hi, lo;
    std::uint16_t current_reg_address = f16_req_starting_address;
    std::uint16_t reg_count = 0;
    std::uint16_t reg_val ;
    std::uint8_t byte_ix = 0;
    while(byte_ix < f16_req_byte_count &&
          reg_count < f16_req_quantity_of_regs )
    {
        hi = data[byte_ix];
        byte_ix++;
        lo = data[byte_ix];
        byte_ix++;
        reg_val = mb_scanner::combine(hi, lo);
        set_reg(current_reg_address, reg_val);
        current_reg_address++;
        reg_count++;
    }
    mb_scanner::get_hi_lo(f16_req_starting_address, hi, lo);
    m_output_buffer.add(hi);
    m_output_buffer.add(lo);
    mb_scanner::get_hi_lo(f16_req_quantity_of_regs, hi, lo);
    m_output_buffer.add(hi);
    m_output_buffer.add(lo);
    add_crc();
    send_pdu();
}

void rtu_server_data_handler_c::reply_f23(std::uint8_t fc,
                   std::uint16_t f23_req_read_starting_address,
                   std::uint16_t f23_req_quantity_to_read,
                   std::uint16_t f23_req_write_starting_address,
                   std::uint16_t f23_req_quantity_to_write,
                   std::uint8_t f23_req_write_byte_count,
                   const mb_scanner::uchar_vect_t &data)
{
    // first write
    std::uint8_t hi, lo;
    std::uint16_t current_reg_address = f23_req_write_starting_address;
    std::uint16_t reg_count = 0;
    std::uint16_t reg_val ;
    std::uint8_t byte_ix = 0;
    while(byte_ix < f23_req_write_byte_count &&
          reg_count < f23_req_quantity_to_write )
    {
        hi = data[byte_ix];
        byte_ix++;
        lo = data[byte_ix];
        byte_ix++;
        reg_val = mb_scanner::combine(hi, lo);
        set_reg(current_reg_address, reg_val);
        current_reg_address++;
        reg_count++;
    }
    // then read
    reply_f3(fc, f23_req_read_starting_address,
                 f23_req_quantity_to_read);

}


void rtu_server_data_handler_c::add_crc()
{
    uint8_t crc_hi, crc_lo;
    mb_scanner::calc_crc_rtu(m_output_buffer, crc_hi, crc_lo);
    m_output_buffer.add(crc_hi);
    m_output_buffer.add(crc_lo);
}

void rtu_server_data_handler_c::handle_writing(const boost::system::error_code error)
{
    start_read();
}

void rtu_server_data_handler_c::send_pdu()
{
    #ifdef VERBOSE
    std::cout << "send " << to_hex(m_output_buffer, m_output_buffer.actual_size()) << std::endl;
    #endif
    boost::asio::async_write(
        m_serial_port,
        boost::asio::buffer(m_output_buffer, m_output_buffer.actual_size()),
        boost::bind(&rtu_server_data_handler_c::handle_writing,
        shared_from_this(),
        boost::asio::placeholders::error));
}

void rtu_server_data_handler_c::mirror_pdu()
{
    boost::asio::async_write(
        m_serial_port,
        boost::asio::buffer(m_input_buffer, m_input_buffer.actual_size()),
        boost::bind(&rtu_server_data_handler_c::handle_writing,
        shared_from_this(),
        boost::asio::placeholders::error));
}

// scanner part
void rtu_server_data_handler_c::on_pdu(const mb_scanner::pdu_c& pdu)
{
        std::cout << "rtu_server_data_handler_c::on_pdu f " << (int)pdu.m_fc;
        switch( pdu.m_fc)
        {
        case 1:
            std::cout << "\n F1_STARTING_ADDRESS  " << (int)pdu.F1_REQ_STARTING_ADDRESS
                      << "\n F1_QUANTITY_OF_COILS " << (int)pdu.F1_REQ_QUANTITY_OF_COILS 
                      << std::endl;
            reply_f1(1,
                     pdu.F1_REQ_STARTING_ADDRESS, 
                     pdu.F1_REQ_QUANTITY_OF_COILS);
            break;
        case 2:
            std::cout << "\n F2_REQ_STARTING_ADDRESS  " << (int)pdu.F2_REQ_STARTING_ADDRESS
                      << "\n F2_REQ_QUANTITY_OF_INPUTS " << (int)pdu.F2_REQ_QUANTITY_OF_INPUTS 
                      << std::endl;
            reply_f2(2,
                     pdu.F2_REQ_STARTING_ADDRESS, 
                     pdu.F2_REQ_QUANTITY_OF_INPUTS);
            break;
        case 3:
            std::cout << "\n F3_REQ_STARTING_ADDRESS  " << (int)pdu.F3_REQ_STARTING_ADDRESS
                      << "\n F3_REQ_QUANTITY_OF_REGS  " << (int)pdu.F3_REQ_QUANTITY_OF_REGS
                      << std::endl;
            reply_f3(3, 
                     pdu.F3_REQ_STARTING_ADDRESS, 
                     pdu.F3_REQ_QUANTITY_OF_REGS);
            break;
        case 4:
            std::cout << "\n F4_REQ_STARTING_ADDRESS " <<  (int)pdu.F4_REQ_STARTING_ADDRESS
                      << "\n F4_REQ_QUANTITY_OF_REGS " << (int)pdu.F4_REQ_QUANTITY_OF_REGS
                      << std::endl;
            reply_f4(4, 
                     pdu.F4_REQ_STARTING_ADDRESS, 
                     pdu.F4_REQ_QUANTITY_OF_REGS);
            break;
        case 5:
            std::cout << "\n F5_REQ_OUTPUT_ADDRESS " <<  (int)pdu.F5_REQ_OUTPUT_ADDRESS
                      << "\n F5_REQ_OUTPUT_VALUE " << (int)pdu.F5_REQ_OUTPUT_VALUE
                      << std::endl;
            reply_f5(5, 
                     pdu.F5_REQ_OUTPUT_ADDRESS, 
                     pdu.F5_REQ_OUTPUT_VALUE);
            break;
        case 6:
            std::cout << "\n F6_REQ_REG_ADDRESS " << (int)pdu.F6_REQ_REG_ADDRESS
                      << "\n F6_REQ_REG_VALUE " << (int)pdu.F6_REQ_REG_VALUE
                      << std::endl;
            reply_f6(6,
                     pdu.F6_REQ_REG_ADDRESS, 
                     pdu.F6_REQ_REG_VALUE);
            break;
        case 15:
            std::cout <<  "\n F15_REQ_STARTING_ADDRESS " << (int)pdu.F15_REQ_STARTING_ADDRESS
                      <<  "\n F15_REQ_QUANTITY_OF_OUTPUTS " << (int)pdu.F15_REQ_QUANTITY_OF_OUTPUTS
                      <<  "\n F15_REQ_BYTE_COUNT " << (int)pdu.F15_REQ_BYTE_COUNT
                      << std::endl;
            std::cout << "values: " << mb_scanner::to_hex(pdu.m_data) << std::endl;
            reply_f15(15,
                      pdu.F15_REQ_STARTING_ADDRESS,
                      pdu.F15_REQ_QUANTITY_OF_OUTPUTS,
                      pdu.F15_REQ_BYTE_COUNT,
                      pdu.m_data);
            break;
        case 16:
            std::cout << "\n F16_REQ_STARTING_ADDRESS " << (int)pdu.F16_REQ_STARTING_ADDRESS
                      << "\n F16_REQ_QUANTITY_OF_REGS " << (int)pdu.F16_REQ_QUANTITY_OF_REGS
                      << "\n F16_REQ_BYTE_COUNT " << (int)pdu.F16_REQ_BYTE_COUNT
                      << std::endl;
            std::cout << "values: " << mb_scanner::to_hex(pdu.m_data) << std::endl;
            reply_f16(16,
                      pdu.F16_REQ_STARTING_ADDRESS,
                      pdu.F16_REQ_QUANTITY_OF_REGS,
                      pdu.F16_REQ_BYTE_COUNT,
                      pdu.m_data);
            break;
        case 23:
            std::cout << "\n F23_REQ_READ_STARTING_ADDRESS " << (int)pdu.F23_REQ_READ_STARTING_ADDRESS
                      << "\n F23_REQ_QUANTITY_TO_READ " << (int)pdu.F23_REQ_QUANTITY_TO_READ
                      << "\n F23_REQ_WRITE_STARTING_ADDRESS " << (int)pdu.F23_REQ_WRITE_STARTING_ADDRESS
                      << "\n F23_REQ_QUANTITY_TO_WRITE " << (int)pdu.F23_REQ_QUANTITY_TO_WRITE
                      << "\n F23_REQ_WRITE_BYTE_COUNT " << (int)pdu.F23_REQ_WRITE_BYTE_COUNT
                      << std::endl;
            std::cout << "values: " << mb_scanner::to_hex(pdu.m_data) << std::endl;
            reply_f23(23,
                      pdu.F23_REQ_READ_STARTING_ADDRESS,
                      pdu.F23_REQ_QUANTITY_TO_READ,
                      pdu.F23_REQ_WRITE_STARTING_ADDRESS,
                      pdu.F23_REQ_QUANTITY_TO_WRITE,
                      pdu.F23_REQ_WRITE_BYTE_COUNT,
                      pdu.m_data);
            break;
        default:
            start_read();
        }
}

void rtu_server_data_handler_c::on_error(int err)
{
    std::cout << "user::on_err " << err << std::endl;
}

void rtu_server_data_handler_c::on_nonse()
{
    std::cout << "user::on_nonsense " << std::endl;
}

// static
void rtu_server_data_handler_c::on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu)
{
    rtu_server_data_handler_c *p_this{(rtu_server_data_handler_c*)obj};
    p_this->on_pdu(pdu);
}

// static
void rtu_server_data_handler_c::on_error_aux(void* obj, int err)
{
    rtu_server_data_handler_c *p_this{(rtu_server_data_handler_c*)obj};
    p_this->on_error(err);
}

// static
void rtu_server_data_handler_c::on_nonse_aux(void* obj)
{
    rtu_server_data_handler_c *p_this{(rtu_server_data_handler_c*)obj};
    p_this->on_nonse();
}

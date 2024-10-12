#include <memory>
#include <map>

#include <boost/asio.hpp>
#include "../modbus_scanner.h"


class rtu_server_data_handler_c
    : public std::enable_shared_from_this<rtu_server_data_handler_c>
{
    std::map<uint16_t, bool> m_coils;
    std::map<uint16_t, uint16_t> m_regs;

    void *m_p_modbus_scanner;
    boost::asio::serial_port m_serial_port;
    std::string m_port_name;
    mb_scanner::buffer_c m_input_buffer;
    mb_scanner::buffer_c m_output_buffer;

    boost::asio::steady_timer m_sec_timer;
    std::uint8_t m_dev_address;

    void on_pdu(const mb_scanner::pdu_c& pdu);
    void on_error(int err);
    void on_nonse();

    void schedule_timer();
    void handle_timer(const boost::system::error_code& error);
    bool set_port_config(int fd);
    bool open_port();


    void start_read();
    void handle_read(const boost::system::error_code error,
                  const std::size_t length);

    void send_pdu();
    void mirror_pdu();
    void handle_writing(const boost::system::error_code error);
    void add_crc();
    bool get_coil(std::uint16_t index );
    void set_coil(std::uint16_t index, std::uint16_t val );
    uint16_t get_reg(std::uint16_t index );
    void set_reg(std::uint16_t index, std::uint16_t val );

    void reply_header(std::uint8_t fc);

    void reply_f1(std::uint8_t fc,
                  std::uint16_t f1_req_starting_address,
                  std::uint16_t f1_req_quantity_of_coils);

    void reply_f2(std::uint8_t fc,
                  std::uint16_t f2_req_starting_address,
                  std::uint16_t f2_req_quantity_of_inputs);

    void reply_f3(std::uint8_t fc,
                  std::uint16_t f3_req_starting_address,
                  std::uint16_t f3_req_quantity_of_regs);

    void reply_f4(std::uint8_t fc,
                  std::uint16_t f4_req_starting_address,
                  std::uint16_t f4_req_quantity_of_regs);

    void reply_f5(std::uint8_t fc,
                  std::uint16_t f5_req_output_address,
                  std::uint16_t f5_req_output_value);

    void reply_f6(std::uint8_t fc,
                  std::uint16_t f6_req_reg_address,
                  std::uint16_t f6_req_reg_value);

    void reply_f15(std::uint8_t fc,
                   std::uint16_t f15_req_starting_address,
                  std::uint16_t f15_req_quantity_of_outputs,
                  std::uint8_t f15_req_byte_count,
                  const mb_scanner::uchar_vect_t &data);

    void reply_f16(std::uint8_t fc,
                   std::uint16_t f16_req_starting_address,
                   std::uint16_t f16_req_quantity_of_regs,
                   std::uint8_t f16_req_byte_count,
                   const mb_scanner::uchar_vect_t &data);

    void reply_f23(std::uint8_t fc,
                   std::uint16_t f23_req_read_starting_address,
                   std::uint16_t f23_req_quantity_to_read,
                   std::uint16_t f23_req_write_starting_address,
                   std::uint16_t f23_req_quantity_to_write,
                   std::uint8_t f23_req_write_byte_count,
                   const mb_scanner::uchar_vect_t &data);


public:
    rtu_server_data_handler_c(boost::asio::io_service &io_service,
        std::string port_name,
        uint8_t dev_address);
    virtual ~rtu_server_data_handler_c();

    static void on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu);
    static void on_error_aux(void* obj, int err);
    static void on_nonse_aux(void* obj);
    bool start();
};

using sp_rtu_server_data_handler_c = std::shared_ptr<rtu_server_data_handler_c>;

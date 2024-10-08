#pragma once

#include <list>
#include <string>
#include <memory>

#include "../modbus_scanner.h"

class server_connection_c : public std::enable_shared_from_this<server_connection_c>
{
    void handle_read(const boost::system::error_code error,
                     const std::size_t length);
public:
    server_connection_c(boost::asio::ip::tcp::socket sock);
    virtual ~server_connection_c();
    void start_read();

// scanner part
private:
    void *m_p_modbus_scanner;
    boost::asio::ip::tcp::socket m_socket;
    mb_scanner::buffer_c m_buffer;

    void on_pdu(const mb_scanner::pdu_c& pdu);
    void on_error(int err);
    void on_nonse();

public:
    static void on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu);
    static void on_error_aux(void* obj, int err);
    static void on_nonse_aux(void* obj);


};
using sp_server_connection_c = std::shared_ptr<server_connection_c>;

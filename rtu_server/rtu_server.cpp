#include <iostream>
#include <vector>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

#include "rtu_server_data_handler.h"

int main(int argc, char * argv[] )
{
    std::string port_name = "/dev/pts/5"; // "/dev/ttyS11"; 
    int o = getopt(argc, argv, "p:");
    if ('p' == o )
    {
        port_name = optarg;
    }
    uint8_t dev_addr = 17;
    boost::asio::io_service io_service;
    sp_rtu_server_data_handler_c sp_data_handler( new rtu_server_data_handler_c(io_service, port_name, dev_addr) );
    if ( !sp_data_handler->start() )
    {
        std::cout << "start failed \n";
        return 0;
    }
    std::cout << "start success" << std::endl;
    io_service.run();
    // finally
    return 0;
}

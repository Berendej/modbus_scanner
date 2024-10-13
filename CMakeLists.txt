cmake_minimum_required( VERSION 3.10 )
project( modbus_scanner )

find_package( Boost COMPONENTS context )

add_executable( test_scanner test_scanner.cpp )

add_executable( tcp_server ./tcp_server/tcp_server.cpp ./tcp_server/server_connection.cpp )

add_executable( rtu_server ./rtu_server/rtu_server.cpp ./rtu_server/rtu_server_data_handler.cpp )

add_library( modbus_scanner modbus_scanner.cpp 
             slave_rtu_scanner.cpp 
             slave_tcp_scanner.cpp
             scanner.cpp 
             crc_check.cpp 
             hex_utils.cpp
             slave_rtu_old.cpp )

target_link_libraries( modbus_scanner ${Boost_LIBRARIES} )
target_link_libraries( test_scanner PRIVATE modbus_scanner )
target_link_libraries( tcp_server PRIVATE modbus_scanner )
target_link_libraries( rtu_server PRIVATE modbus_scanner )
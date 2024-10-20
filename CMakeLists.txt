cmake_minimum_required( VERSION 3.10 )
project( modbus_scanner VERSION 0.1.1)

find_package( Boost COMPONENTS context )

add_library( modbus_scanner modbus_scanner.cpp 
             slave_rtu_scanner.cpp 
             slave_tcp_scanner.cpp
             scanner.cpp 
             crc_check.cpp 
             hex_utils.cpp
             slave_rtu_old.cpp )

add_executable( speed_test speed_test.cpp )

add_executable( test_scanner mb_scanner_test.cpp )

add_executable( tcp_server_example ./tcp_server/tcp_server.cpp ./tcp_server/server_connection.cpp )

add_executable( rtu_server_example ./rtu_server/rtu_server.cpp ./rtu_server/rtu_server_data_handler.cpp )

target_link_libraries( modbus_scanner ${Boost_LIBRARIES} )
target_link_libraries( speed_test PRIVATE modbus_scanner )
target_link_libraries( rtu_server_example PRIVATE modbus_scanner )
target_link_libraries( tcp_server_example PRIVATE modbus_scanner )
target_link_libraries( test_scanner PRIVATE modbus_scanner ${Boost_LIBRARIES} )

# package part which enables
# run: cmake --build . --target package
install(TARGETS modbus_scanner ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_CONTACT berrendej@gmail.com)
include(CPack)

# cmake --build . --target test
enable_testing()
add_test(md_scanner_test  test_scanner)

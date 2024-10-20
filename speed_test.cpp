#include <iostream>
#include <chrono>

#include "modbus_scanner.h"

#define COMPARE_SPEED 1

class scanner_user_c
{
    void on_pdu(const mb_scanner::pdu_c& pdu)
    {
    #ifdef COMPARE_SPEED
        return; // for not to messs
    #else
        std::cout << "user::on_pdu f " << (int)pdu.m_fc;
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
        #endif // COMPARE_SPEED
    }

    void on_error(int err)
    {
        std::cout << "user::on_err " << err << std::endl;
    }

    void on_nonse()
    {
        std::cout << "user::on_nonsense " << std::endl;
    }

public:
    static void on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu);
    static void on_error_aux(void* obj, int err);
    static void on_nonse_aux(void* obj);
};

void scanner_user_c::on_pdu_aux(void* obj, const mb_scanner::pdu_c& pdu)
{
    scanner_user_c *p_this{(scanner_user_c*)obj};
    p_this->on_pdu(pdu);
}

void scanner_user_c::on_error_aux(void* obj, int err)
{
    scanner_user_c *p_this{(scanner_user_c*)obj};
    p_this->on_error(err);
}

void scanner_user_c::on_nonse_aux(void* obj)
{
    scanner_user_c *p_this{(scanner_user_c*)obj};
    p_this->on_nonse();
}

void check_slave_rtu(bool cor)
{
    mb_scanner::buffer_c v  { 0x1, 0x3, 0x0, 0x0, 0x0, 0x2, 196, 11 };
    mb_scanner::buffer_c v1 { 0x1, 0x3                              };
    mb_scanner::buffer_c v2 {           0x0, 0x0                    };
    mb_scanner::buffer_c v3 {                     0x0, 0x2          };
    mb_scanner::buffer_c v4 {                               196, 11 };
  //                          0    1    2    3    4     5  6  7  8   9  0  1  2
    mb_scanner::buffer_c f15 { 0x1, 0xf, 0x0, 0x0, 0x0, 0x2, 4, 1, 2, 3, 4, 99, 5};

    // f16 split to pieces
    mb_scanner::buffer_c f16_1 { 0x11, 0x10, 0x00, 0x00, 0x00, 0x63, 0xC6, 0xD7, 0x15, 
        0x64, 0xF5, 0xC8, 0x78, 0xCC, 0x65, 0xE9, 0x60, 0x32, 0x92, 0x55, 0xD0, 0xC4, 
        0xA9, 0x47, 0x1B, 0x8D, 0xD0, 0x7A, 0x36, 0xA0, 0xFD, 0x5D, 0x62, 0x83, 0x6D};
    mb_scanner::buffer_c f16_2 { 0xF3, 0xC4, 0xEA, 0x8A, 0xA2, 0xBD, 0xB7, 0xA0, 0x24, 
        0x3F, 0x9B, 0x61, 0x04, 0x2C, 0x3E, 0x2D, 0x23, 0x21, 0xCD, 0xDD, 0x28, 0x1B, 
        0x66, 0xA3, 0x21, 0x39, 0x1B, 0xDA, 0xFF, 0xB8, 0x37, 0xDF, 0x83, 0x4F, 0xD6, 
        0xCF, 0x9C, 0xD5, 0x4B, 0xC8, 0xA3, 0x35, 0x86, 0x37, 0x7E, 0x5A, 0xF9, 0x06, 
        0x4A, 0xE2, 0xC5, 0x76, 0x86, 0xD8, 0xC5, 0x18, 0x66, 0x74, 0xE4, 0x3A, 0x48};
    mb_scanner::buffer_c f16_3 { 0x87, 0x5A, 0x3A, 0xCE, 0xC6, 0xEB, 0x44, 0x11, 0xDB, 
    0xF3, 0x06, 0x86, 0xA7, 0x16, 0x07, 0x31, 0x34, 0xA9, 0xC8, 0xE3, 0xE5, 0x59, 0x50,
     0x10, 0x6D, 0x05, 0x20, 0x75, 0x2B, 0x10, 0x26, 0x3C, 0xFF, 0xF8, 0x7A, 0xE6, 0xF6, 
     0xD9, 0xD5, 0x44, 0x43, 0x8A, 0x2D, 0x60, 0x0D, 0xC2, 0x9E, 0x83, 0x35, 0xAA, 0xEF, 
     0x88, 0x16, 0x0A, 0x0E, 0x70, 0x08, 0xEE, 0x8B, 0xEE, 0x48, 0xB8, 0x8F, 0x48, 0xC7, 
     0xBD, 0x0F, 0xA3, 0xD5, 0x5A, 0xA2, 0xB0, 0x16, 0x2A, 0x7D, 0x70, 0xAA, 0xE1, 0x4B};
     mb_scanner::buffer_c f16_4 { 0xD4, 0x45, 0x54, 0x90, 0x3A, 0x9D, 0xE4, 0xB3, 0x59, 
     0xB0, 0xAF, 0xC8, 0xF4, 0xDA, 0x96, 0xB0, 0xA8, 0x44, 0xDB, 0xD1, 0x70, 0x86, 0xEC, 
     0x88, 0x65, 0xFF, 0xD0, 0x93, 0xAF, 0x28, 0x66, 0xFF};

     mb_scanner::buffer_c f23_1{ 0x11, 0x17, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00,
      0x63, 0xC6, 0x28, 0xEA, 0x9B, 0x0A, 0x37, 0x87, 0x33, 0x9A, 0x16, 0x9F, 0xCD, 0x6D, 
      0xAA, 0x2F, 0x3B, 0x56, 0xB8, 0xE4, 0x72, 0x2F, 0x85, 0xC9, 0x5F, 0x02, 0xA2, 0x9D, 
      0x7C, 0x92, 0x0C, 0x3B, 0x15, 0x75, 0x5D, 0x42, 0x48, 0x5F, 0xDB, 0xC0, 0x64, 0x9E, 
      0xFB, 0xD3, 0xC1, 0xD2, 0xDC, 0xDE, 0x32, 0x22, 0xD7, 0xE4, 0x99, 0x5C, 0xDE, 0xC6, 
      0xE4, 0x25, 0x00, 0x47, 0xC8, 0x20, 0x7C, 0xB0, 0x29, 0x30, 0x63, 0x2A, 0xB4, 0x37};

      mb_scanner::buffer_c f23_2 { 0x5C, 0xCA, 0x79, 0xC8, 0x81, 0xA5, 0x06, 0xF9, 0xB5, 
      0x1D, 0x3A, 0x89, 0x79, 0x27, 0x3A, 0xE7, 0x99, 0x8B, 0x1B, 0xC5, 0xB7, 0x78, 0xA5, 
      0xC5, 0x31, 0x39, 0x14, 0xBB, 0xEE, 0x24, 0x0C, 0xF9, 0x79, 0x58, 0xE9, 0xF8, 0xCE, 
      0xCB, 0x56, 0x37, 0x1C, 0x1A, 0xA6, 0xAF, 0xEF, 0x92, 0xFA, 0xDF, 0x8A, 0xD4, 0xEF, 
      0xD9, 0xC3, 0x00, 0x07, 0x85, 0x19, 0x09, 0x26, 0x2A, 0xBB, 0xBC, 0x75, 0xD2, 0x9F};

      mb_scanner::buffer_c f23_3 { 0xF2, 0x3D, 0x61, 0x7C, 0xCA, 0x55, 0x10, 0x77, 0xE9, 
      0xF5, 0xF1, 0x8F, 0xF7, 0x11, 0x74, 0x11, 0xB7, 0x47, 0x70, 0xB7, 0x38, 0x42, 0xF0,
       0x5C, 0x2A, 0xA5, 0x5D, 0x4F, 0xE9, 0xD5, 0x82, 0x8F, 0x55, 0x1E, 0xB4, 0x2B, 0xBA, 
       0xAB, 0x6F, 0xC5, 0x62, 0x1B, 0x4C, 0xA6, 0x4F, 0x50, 0x37, 0x0B, 0x25, 0x69, 0x4F, 
       0x57, 0xBB, 0x24, 0x2E, 0x8F, 0x79, 0x13, 0x77, 0x9A, 0x00, 0x2F, 0x6C, 0x50, 0xD7, 
       0x87, 0x45 };


    scanner_user_c user;
    void* scanner ;
    if ( cor )
    {
        scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_rtu, 17,
        scanner_user_c::on_pdu_aux,
        scanner_user_c::on_error_aux,
        scanner_user_c::on_nonse_aux,
        &user);
    }
    else
    {
        scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_rtu_old, 17,
        scanner_user_c::on_pdu_aux,
        scanner_user_c::on_error_aux,
        scanner_user_c::on_nonse_aux,
        &user);
    }

    const auto start = std::chrono::system_clock::now();

    const int cycles = 100000;
    for( auto ix = 0; ix < cycles; ++ix )
    {// body of the test to measure
        mb_scanner::scan(scanner, &f16_1 );
        mb_scanner::scan(scanner, &f16_2 );
        mb_scanner::scan(scanner, &f16_3 );
        mb_scanner::scan(scanner, &f16_4 );
        mb_scanner::scan(scanner, &f23_1 );
        mb_scanner::scan(scanner, &f23_2 );
        mb_scanner::scan(scanner, &f23_3 );
    }
    const auto stop = std::chrono::system_clock::now();
    const auto secs = std::chrono::duration<double>(stop - start);
    std::cout << (cor? "coroutine ": "virt f ") <<  secs.count()
              << std::endl;
    mb_scanner::delete_scanner(scanner);
}

void check_slave_tcp()
{
    mb_scanner::buffer_c v { 0x0, 0x1, // transact = 1
                                 0x0, 0x0, // protocol = 0
                                 0x0, 0x6, // length
                                 0x1, // unit id
                                 0x3,  // fc
                                 0x0, 0x6, // first reg = 6
                                 0x0, 0x2  // num of regs 11 
                               };
#if 0
    mb_scanner::buffer_c v1 { 0x1, 0x3                              };
    mb_scanner::buffer_c v2 {           0x0, 0x0                    };
    mb_scanner::buffer_c v3 {                     0x0, 0x2          };
    mb_scanner::buffer_c v4 {                               196, 11 };
    mb_scanner::buffer_c f15 { 0x1, 0xf, 0x0, 0x0, 0x0, 0x2, 4, 1, 2, 3, 4, 99, 5};
#endif
    scanner_user_c user;
    void* scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_tcp, 1,
        scanner_user_c::on_pdu_aux,
        scanner_user_c::on_error_aux,
        scanner_user_c::on_nonse_aux,
        &user);
    mb_scanner::scan(scanner, &v );
#if 0
    mb_scanner::scan(scanner, &v );
    mb_scanner::scan(scanner, &v1);
    mb_scanner::scan(scanner, &v2);
    mb_scanner::scan(scanner, &v3);
    mb_scanner::scan(scanner, &v4);
#endif
    mb_scanner::delete_scanner(scanner);
}

int main(int argc, char* argv[])
{
    check_slave_tcp(); // to warm up
    check_slave_rtu(false); // old
    check_slave_rtu(true);  // coroutine
}
 
#include <iostream>

#include "defs.h"
#include "modbus_scanner.h"

class scanner_user_c
{
    void on_pdu(const mb_scanner::pdu_c& pdu)
    {
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

void check_slave_rtu()
{
    mb_scanner::buffer_c v  { 0x1, 0x3, 0x0, 0x0, 0x0, 0x2, 196, 11 };

    mb_scanner::buffer_c v1 { 0x1, 0x3                              };

    mb_scanner::buffer_c v2 {           0x0, 0x0                    };

    mb_scanner::buffer_c v3 {                     0x0, 0x2          };

    mb_scanner::buffer_c v4 {                               196, 11 };

  //                          0    1    2    3    4     5  6  7  8   9  0  1  2
   mb_scanner::buffer_c f15 { 0x1, 0xf, 0x0, 0x0, 0x0, 0x2, 4, 1, 2, 3, 4, 99, 5};

    scanner_user_c user;

    void* scanner = mb_scanner::create_scanner(mb_scanner::protocol_e::slave_rtu, 1,
        scanner_user_c::on_pdu_aux,
        scanner_user_c::on_error_aux,
        scanner_user_c::on_nonse_aux,
        &user);

    mb_scanner::scan(scanner, &f15 );

//#if 0
    mb_scanner::scan(scanner, &v );
    mb_scanner::scan(scanner, &v1);
    mb_scanner::scan(scanner, &v2);
    mb_scanner::scan(scanner, &v3);
    mb_scanner::scan(scanner, &v4);
//#endif
    
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
    check_slave_rtu();
    check_slave_tcp();
}
#include "defs.h"
#include "slave_rtu_scanner.h"
#include "slave_tcp_scanner.h"

namespace mb_scanner
{
    void* create_scanner(protocol_e prot, 
                        int address, 
                        pdu_callback_t pdu_cb,
                        error_callback_t err_cb,
                        nonsense_callback_t nonse_cb,
                        void *user)
    {
        switch( prot )
        {
        case protocol_e::slave_rtu:
            return (void*) new slave_rtu_scanner_c(address, 
                                                   pdu_cb, err_cb, 
                                                   nonse_cb, user );
            break;
        case protocol_e::slave_tcp:
            return (void*) new slave_tcp_scanner_c(address, 
                                                   pdu_cb, err_cb, 
                                                   nonse_cb, user );
            break;
        /* not now
        case protocol_e::ascii:
            return (void*) new ascii_scanner_c(address, pdu_cb, err_cb, user );
        case protocol_e::tcp:
            return (void*) new tcp_scanner_c(address, pdu_cb, err_cb, user );
        */
        }
        return nullptr;
    }

    void delete_scanner(void *scanner)
    {
        scanner_c *p_scanner { (scanner_c*) scanner};
        delete p_scanner;
    }

    bool scan(void *scanner, const buffer_c *p_input )
    {
        scanner_c *p_scanner { (scanner_c*) scanner};
        p_scanner->scan(p_input);
        return true;
    }
} // namespace mb_scanner



#ifndef _OPENNUI_SYSTEM_COMM_PROTOCOL_H_
#define _OPENNUI_SYSTEM_COMM_PROTOCOL_H_

#include <vee/enumeration.h>
#include <array>

namespace opennui {

namespace sys {

namespace comm {

namespace protocol {

Enumeration(comm_port, 12835,
            handshake_port,
            web_comm_port);

Enumeration(opcode_t, 500,
            null,
            handshake_hello,
            end_of_record);

struct message_header
{
    opcode_t opcode = opcode_t::null;
    uint32_t block_size = 0;
    uint32_t message_id = 0;
};

struct message
{
    static const int data_section_max = 512;
    message_header header;
    ::std::array<unsigned char, data_section_max> data;
    inline void clear()
    {
        memset(&header, 0, sizeof(header));
        data.fill(0);
    }
};

void validate_header(message_header& header) throw(...);

} // !namespace 

} // !namespace comm

} // !namespace sys

} // !namepsace opennui

#endif // !_OPENNUI_SYSTEM_COMM_PROTOCOL_H_

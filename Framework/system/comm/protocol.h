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

static const int handshake_packet_size = 512;

Enumeration(opcode_t, 500,
            handshake_hello,
            );

struct message
{
    struct header_section
    {
        opcode_t opcode;
        uint32_t block_size;
        uint32_t message_id;
    };
    static const int header_size = sizeof(header_section);
    static const int data_section_max = 512;
    header_section header;
    ::std::array<unsigned char, data_section_max> data;
    inline void clear()
    {
        memset(&header, 0, sizeof(header));
        data.fill(0);
    }
};

} // !namespace 

} // !namespace comm

} // !namespace sys

} // !namepsace opennui

#endif // !_OPENNUI_SYSTEM_COMM_PROTOCOL_H_

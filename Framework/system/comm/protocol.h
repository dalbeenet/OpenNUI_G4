#ifndef _OPENNUI_SYSTEM_COMM_PROTOCOL_H_
#define _OPENNUI_SYSTEM_COMM_PROTOCOL_H_

#include <vee/enumeration.h>

namespace opennui {

namespace sys {

namespace comm {

namespace protocol {

Enumeration(comm_port, 12835,
            handshake_port,
            web_comm_port);

static const int handshake_packet_size = 512;

} // !namespace 

} // !namespace comm

} // !namespace sys

} // !namepsace opennui

#endif // !_OPENNUI_SYSTEM_COMM_PROTOCOL_H_

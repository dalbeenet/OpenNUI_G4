#include <system/comm/gateway.h>
#include <system/exceptions.h>

namespace opennui {

namespace sys {

namespace comm {

namespace protocol {

void validate_header(message_header& header)
{
    if (header.opcode <= opcode_t::null || header.opcode >= opcode_t::end_of_record)
        throw exceptions::invalid_opcode(header.opcode.to_int32());
    if (header.block_size > message::data_section_max || header.block_size < 0)
        throw exceptions::invalid_block_size(header.block_size);
}

} // !namespace comm

} // !namespace protocol

} // !namespace sys

} // !namespace opennui
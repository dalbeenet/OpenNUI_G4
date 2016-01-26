#include <opennui/comm/protocol.h>
#include <opennui/exceptions.h>

namespace opennui {

namespace protocol {

namespace comm {

void validate_header(message_header& header) throw(...)
{
    if (header.opcode <= opcode_t::null || header.opcode >= opcode_t::end_of_record)
        throw exceptions::invalid_opcode(header.opcode.to_int32());
    if (header.block_size > message::data_section_max || header.block_size < 0)
        throw exceptions::invalid_block_size(header.block_size);
}

} // !namespace protocol

} // !namespace comm

} // !namespace opennui
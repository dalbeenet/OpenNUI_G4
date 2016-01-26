#include <opennui/exceptions.h>
#include <cstdio>

namespace opennui {

namespace protocol {

namespace exceptions {

invalid_opcode::invalid_opcode(uint32_t _opcode)
{
    sprintf_s(_desc, "invalid opcode (opcode: %d)", _opcode);
}

invalid_block_size::invalid_block_size(size_t _block_size)
{
    sprintf_s(_desc, "invalid block size (block size: %d)", _block_size);
}

} // !namespace exceptions

} // !namespace protocol

} // !namespace opennui
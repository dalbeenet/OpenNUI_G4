#include <opennui/exceptions.h>
#include <cstdio>
#include <cstdarg>
#include <array>

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

block_size_too_small::block_size_too_small(size_t _block_size)
{
    sprintf_s(_desc, "invalid block size (block size: %d)", _block_size);
}

handshake_failed::handshake_failed(const char* additional_info, ...)
{
    std::array<char, 1024> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, additional_info);
    vsprintf_s(buffer.data(), buffer.size(), additional_info, ap);
    sprintf_s(_desc, "handshake failed\n\tdetail: %s", buffer.data());
    va_end(ap);
}

} // !namespace exceptions

} // !namespace protocol

} // !namespace opennui
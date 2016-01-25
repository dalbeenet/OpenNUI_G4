#include <system/exceptions.h>
#include <cstdio>

namespace opennui {

namespace sys {

namespace exceptions {

#pragma warning(disable:4996)
module_load_failed::module_load_failed(int _gle):
gle(_gle)
{
    sprintf(_desc, "opennui module load failed (gle: %d)", gle);
}

module_entry_point_not_found::module_entry_point_not_found(const char* module_name, int _gle):
gle(_gle)
{
    sprintf(_desc, "could not found the entry point of opennui module \"%s\" (gle: %d)", gle, module_name);
}

} // !namespace exceptions

namespace comm {

namespace exceptions {

invalid_opcode::invalid_opcode(uint32_t _opcode)
{
    sprintf(_desc, "invalid opcode (opcode: %d)", _opcode);
}

invalid_block_size::invalid_block_size(size_t _block_size)
{
    sprintf(_desc, "invalid block size (block size: %d)", _block_size);
}

} // !namespace exceptions

} // !namespace comm

} // !namespace sys

} // !namespce opennui
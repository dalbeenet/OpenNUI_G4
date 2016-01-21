#include <system/exception.h>
#include <cstdio>

namespace opennui {

namespace system {

namespace exceptions {

#pragma warning(disable:4996)
module_load_failed::module_load_failed(int _gle):
gle(_gle)
{
    sprintf(_desc, "opennui module load failed (gle: %d)", gle);
}

} // !namespace exceptions

} // !namespace system

} // !namespce opennui
#include <system/sysbase.h>

namespace opennui {

namespace sys {

::vee::scheduler<OPENNUI_SYSTEM_TASK_SIG> scheduler(::std::thread::hardware_concurrency(), 256);

} // !namespace sys

} // !namespace opennui
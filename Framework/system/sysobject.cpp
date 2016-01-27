#include <system/sysbase.h>

namespace opennui {

namespace sys {

::vee::scheduler<OPENNUI_SYSTEM_TASK_SIG> scheduler(::std::thread::hardware_concurrency(), 256);


uint32_t generate_taskid()
{
    static int taskid = 100;
    return taskid++;
}

} // !namespace sys

} // !namespace opennui
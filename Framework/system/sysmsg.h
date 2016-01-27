#ifndef _OPENNUI_SYSTEM_SYSMSG_H_
#define _OPENNUI_SYSTEM_SYSMSG_H_

#include <vee/enumeration.h>

namespace opennui {

namespace sys {

Enumeration(sysmsg_id, 1000,
            virtual_device_created,
            virtual_device_removed,
            session_created,
            session_removed);

struct sysmsg
{
    sysmsg_id id;
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_SYSMSG_H_

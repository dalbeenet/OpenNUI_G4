#ifndef _OPENNUI_SYSTEM_DEVICE_MANAGER_H_
#define _OPENNUI_SYSTEM_DEVICE_MANAGER_H_

#include <system/virutual_device.h>
#include <vee/filesystem/path.h>

namespace opennui {

namespace sys {

class device_loader
{
public:
    ~device_loader() = default;
    static ::std::vector<::std::string> scan();
    static ::std::vector<virtual_device::shared_ptr> create_all();
private:
    static device_loader& _get_instance();
    device_loader() = default;
};

class device_manager
{
public:
    ~device_manager();
private:
    static device_manager& _get_instance();
    device_manager() = default;
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_DEVICE_MANAGER_H_

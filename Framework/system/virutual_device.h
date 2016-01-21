#ifndef _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
#define _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_

#include <vee/base.h>
#include <opennui/opennui.h>
#include <system/exception.h>
#include <string>
#include <memory>

namespace opennui {

namespace system {

class virtual_device
{
    using module_ptr = opennui_device*;
public:

    using key_t = uint32_t;
    using shared_ptr = ::std::shared_ptr<virtual_device>;
    virtual_device() = delete;
    // constructors
    virtual_device(const char* module_path) throw(...);
    virtual_device(const virtual_device& other);
    virtual_device(virtual_device&& other);
    // destructor
    ~virtual_device();
    // assignment operator
    virtual_device& operator=(const virtual_device& other);
    // move assignment operator
    virtual_device& operator=(virtual_device&& other);
    
    inline const char* name()
    {
        return _name.c_str();
    }

private:
    void _open(const char* module_path) throw(...);

private:
    key_t _key;
    module_ptr _module;
    ::std::string _name;
    nuidata::image::image_format _color_frame_format;
    nuidata::image::image_format _depth_frame_format;
    nuidata::tracking::body::tracking_info _body_tracking_info;
};

} // !namespace system

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
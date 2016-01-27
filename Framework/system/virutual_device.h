#ifndef _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
#define _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_

#include <system/sysbase.h>
#include <system/exceptions.h>
#include <opennui/service/shared_buffer.h>
#include <string>
#include <memory>

#if OPENNUI_PLATFORM_WINDOWS
#ifndef DECLARE_HANDLE
#define _OPENNUI_DEF_DECLAER_HANDLE
#if 0 && (_MSC_VER > 1000)
#define DECLARE_HANDLE(name) struct name##__; typedef struct name##__ *name
#else
#define DECLARE_HANDLE(name) struct name##__; typedef struct name##__ *name
#endif
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCEs */
#endif // !DECLARE_HANDLE
#endif // !OPENNUI_PLATFORM_WINDOWS

namespace opennui {

namespace sys {

class virtual_device
{
    virtual_device() = delete;
    DISALLOW_COPY_AND_ASSIGN(virtual_device);
    DISALLOW_MOVE_AND_ASSIGN(virtual_device);
    using module_ptr = opennui_device*;
public:
#if OPENNUI_PLATFORM_WINDOWS
    using native_handle = HMODULE;
#ifdef _OPENNUI_DEF_DECLAER_HANDLE
#undef DECLARE_HANDLE
#endif // !_CUSTOM_DEF_DECLAER_HANDLE
#else
    // Linux and others...
#endif // !OPENNUI_PLATFORM_WINDOWS
    using key_t = uint32_t;
    using shared_ptr = ::std::shared_ptr<virtual_device>;
    
    // constructors
    virtual_device(const char* module_path) throw(...);
    // destructor
    ~virtual_device();
    
    inline const key_t key()
    {
        return _key;
    }
    inline const module_ptr module_addr()
    {
        return _module;
    }
    inline const char* name()
    {
        return _name.c_str();
    }
    inline native_handle native()
    {
        return _native;
    }
    inline shared_buffer::shared_ptr color_frame_buffer()
    {
        return _color_frame_buffer;
    }
    inline shared_buffer::shared_ptr depth_frame_buffer()
    {
        return _depth_frame_buffer;
    }
    inline shared_buffer::shared_ptr body_frame_buffer()
    {
        return _body_frame_buffer;
    }

private:
    void _open(const char* module_path) throw(...);
    void _close() __noexcept;

private:
    key_t _key;
    module_ptr _module;
    ::std::string _name;
    native_handle _native;
    shared_buffer::shared_ptr _color_frame_buffer;
    shared_buffer::shared_ptr _depth_frame_buffer;
    shared_buffer::shared_ptr _body_frame_buffer;
    nuidata::image::image_format _color_frame_format;
    nuidata::image::image_format _depth_frame_format;
    nuidata::tracking::body::tracking_info _body_tracking_info;
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
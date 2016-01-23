#ifndef _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
#define _OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_

#include <system/sysbase.h>
#include <system/exceptions.h>
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

namespace system {

class virtual_device
{
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
    virtual_device(virtual_device&& other);
    // destructor
    ~virtual_device();
    // move assignment operator
    virtual_device& operator=(virtual_device&& other);
    
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

private:
    void _open(const char* module_path) throw(...);
    void _close() __noexcept;

    virtual_device() = delete;
    virtual_device(const virtual_device& other) = delete;
    virtual_device& operator=(const virtual_device& other) = delete;

private:
    key_t _key;
    module_ptr _module;
    ::std::string _name;
    native_handle _native;
    nuidata::image::image_format _color_frame_format;
    nuidata::image::image_format _depth_frame_format;
    nuidata::tracking::body::tracking_info _body_tracking_info;
};

} // !namespace system

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_VIRTUAL_DEVICE_H_
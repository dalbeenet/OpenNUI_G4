#include <system/virutual_device.h>
#include <vee/filesystem/path.h>
#include <Windows.h>

namespace opennui {

namespace sys {

virtual_device::virtual_device(const char* module_path) throw(...):
_name(module_path)
{
    _open(module_path);
}

virtual_device::~virtual_device()
{
    _close();
}

virtual_device::virtual_device(virtual_device&& other):
_key(::std::move(other._key)),
_module(::std::move(other._module)),
_name(::std::move(other._name)),
_native(::std::move(other._native)),
_color_frame_format(::std::move(_color_frame_format)),
_depth_frame_format(::std::move(_depth_frame_format)),
_body_tracking_info(::std::move(_body_tracking_info))
{
#if OPENNUI_PLATFORM_WINDOWS
    other._native = NULL;
#else
    // Linux and others...
#endif // !OPENNUI_PLATFORM_WINDOWS
}

virtual_device& virtual_device::operator=(virtual_device&& other)
{
    _close();
    _key = ::std::move(other._key);
    _module = ::std::move(other._module);
    _name = ::std::move(other._name);
    _native = ::std::move(other._native);
    _color_frame_format = ::std::move(_color_frame_format);
    _depth_frame_format = ::std::move(_depth_frame_format);
    _body_tracking_info = ::std::move(_body_tracking_info);
#if OPENNUI_PLATFORM_WINDOWS
    other._native = NULL;
#else
    // Linux and others...
#endif // !OPENNUI_PLATFORM_WINDOWS
    return *this;
}

void virtual_device::_open(const char* module_path) throw(...)
{
    if (::vee::filesystem::exists(module_path) == false || ::vee::filesystem::is_regular_file(module_path) == false)
    {
        throw exceptions::module_not_exist();
    }
    
    HMODULE module_handle = NULL;
    module_handle = LoadLibraryA(module_path);
    if (module_handle == NULL)
    {
        logger::system_error_log("Module load exception occured in %s (gle: %d)", __FUNCTION__, GetLastError());
        throw exceptions::module_load_failed(GetLastError());
    }

#if OPENNUI_PLATFORM_WINDOWS
    using entry_point = module_ptr(__stdcall*)();
    entry_point ep = (entry_point)GetProcAddress(module_handle, "opennui_main");
    if (module_handle == NULL)
    {
        logger::system_error_log("Entry point not found exception occured in %s (gle: %d)", __FUNCTION__, GetLastError());
        throw exceptions::module_entry_point_not_found(module_path, GetLastError());
    }

    _module = ep();
    ::std::hash<::std::string> hash;
    _key = hash(_name);
    _module->name(_name);
    _native = (native_handle)module_handle;
    _module->get_color_frame_format(_color_frame_format);
    _module->get_depth_frame_format(_depth_frame_format);
    _module->get_body_tracking_info(_body_tracking_info);
#else
    // Linux and others...
#endif
}

void virtual_device::_close() __noexcept
{
#if OPENNUI_PLATFORM_WINDOWS
    if (_native != NULL)
    {
        if (FreeLibrary(_native) == 0)
        {
            logger::system_error_log("FreeLibrary() failed in %s (gle: %d)", __FUNCTION__, GetLastError());
        }
    }
#endif
}

} // !namespace sys

} // !namespece opennui
#include <system/virutual_device.h>
#include <vee/filesystem/path.h>
#include <Windows.h>

namespace opennui {

namespace system {

virtual_device::virtual_device(const char* module_path) throw(...):
_name(module_path)
{

}

virtual_device::~virtual_device()
{

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
        throw exceptions::module_load_failed(GetLastError());
    }

    using entry_point = module_ptr(__stdcall*)();
    entry_point ep = (entry_point)GetProcAddress(module_handle, "opennui_main");
    if (module_handle == NULL)
    {
        throw exceptions::module_load_failed(GetLastError());
    }

    //TODO: mapping
}

} // !namespace system

} // !namespece opennui
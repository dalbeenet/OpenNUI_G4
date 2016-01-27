#include <system/device_manager.h>
#include <vee/filesystem/path.h>

namespace opennui {

namespace sys {

#pragma region implementation of device_loader class
::std::vector<::std::string> device_loader::scan()
{
    //TODO: Setup file
    namespace fs = ::vee::filesystem;
    auto files = fs::get_file_list("modules", false, true);
    ::std::vector<::std::string> ret;
    for (auto& file : files)
    {
        if (file.find(".nuidevice") != ::std::string::npos)
            ret.push_back(file);
    }
    return ret;
}

::std::vector<virtual_device::shared_ptr> device_loader::create_all()
{
    ::std::vector<virtual_device::shared_ptr> ret;
    try
    {
        auto list_modules = opennui::sys::device_loader::scan();
        for (auto& module_path : list_modules)
        {
            logger::system_log("Load module \"%s\"", module_path.c_str());
            ret.push_back(::std::make_shared<virtual_device>(module_path.c_str()));
        }
    }
    catch (::vee::exception& e)
    {
        logger::system_error_log(e.to_string());
    }
    return ret;
}

device_loader& device_loader::_get_instance()
{
    static device_loader unique_instance;
    return unique_instance;
}
#pragma endregion !implementation of device_loader class

#pragma region implementation of device_manager class
device_manager& device_manager::_get_instance()
{
    static device_manager unique_instance;
    return unique_instance;
}
#pragma endregion !implementation of device_manager class

} // !namespace sys

} // !namespace opennui
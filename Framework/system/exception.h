#ifndef _OPENNUI_SYSTEM_EXCEPTION_H_
#define _OPENNUI_SYSTEM_EXCEPTION_H_

#include <vee/io/exception.h>

namespace opennui {

namespace system {

namespace exceptions {

class module_not_exist: public ::vee::exceptions::file_open_failed
{
    inline const char* what()
    {
        return "module not exist";
    }
};

class module_load_failed: public module_not_exist
{
    char _desc[256];
public:
    explicit module_load_failed(int _gle);
    inline const char* what()
    {
        return _desc;
    }
    int  gle;
};

} // !namespace exceptions

} // !namespace system

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_EXCEPTION_H_

#ifndef _OPENNUI_SYSTEM_EXCEPTIONS_H_
#define _OPENNUI_SYSTEM_EXCEPTIONS_H_

#include <vee/io/exception.h>

namespace opennui {

namespace system {

namespace exceptions {

class module_not_exist: public ::vee::exceptions::file_open_failed
{
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return "module not exist";
    }
};

class module_load_failed: public ::vee::exception, public ::std::exception
{
    char _desc[256];
public:
    explicit module_load_failed(int _gle);
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return _desc;
    }
    int  gle;
};

class module_entry_point_not_found: public ::vee::exception, public ::std::exception
{
    char _desc[256];
public:
    explicit module_entry_point_not_found(const char* module_name, int _gle);
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return _desc;
    }
    int  gle;
}

} // !namespace exceptions

} // !namespace system

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_EXCEPTIONS_H_

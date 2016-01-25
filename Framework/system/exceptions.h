#ifndef _OPENNUI_SYSTEM_EXCEPTIONS_H_
#define _OPENNUI_SYSTEM_EXCEPTIONS_H_

#include <vee/io/exception.h>
#include <cinttypes>

namespace opennui {

namespace sys {

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
};

} // !namespace exceptions

namespace comm {

namespace exceptions {

class protocol_mismatch: public ::vee::exception, public ::std::exception
{
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return "protocol mismatch";
    }
};

class invalid_opcode: public protocol_mismatch
{
    char _desc[256];
public:
    explicit invalid_opcode(uint32_t _opcode);
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return _desc;
    }
    uint32_t opcode;
};

class invalid_block_size: public protocol_mismatch
{
    char _desc[256];
public:
    explicit invalid_block_size(size_t _block_size);
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return _desc;
    }
    size_t block_size;
};

} // !namespace exceptions

} // !namespace comm

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_EXCEPTIONS_H_

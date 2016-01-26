#ifndef _OPENNUI_SDK4_EXCEPTIONS_H_
#define _OPENNUI_SDK4_EXCEPTIONS_H_

#include <vee/io/exception.h>
#include <cinttypes>

namespace opennui {

namespace protocol {

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

class handshake_failed: public protocol_mismatch
{
    char _desc[1024];
public:
    explicit handshake_failed(const char* additional_info, ...);
    virtual const char* to_string()
    {
        return this->what();
    }
    inline const char* what()
    {
        return _desc;
    }
};

} // !namespace exceptions

} // !namespace protocol

} // !namespace opennui

#endif // !_OPENNUI_SDK4_EXCEPTIONS_H_

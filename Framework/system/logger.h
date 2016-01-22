#ifndef _OPENNUI_SYSTEM_LOGGER_H_
#define _OPENNUI_SYSTEM_LOGGER_H_

#include <string>

namespace opennui {

namespace system {

class logger abstract
{
public:
    static void system_log(std::string& s);
    static void system_log(const char* format, ...);
    static void system_error_log(std::string& s);
    static void system_error_log(const char* format, ...);
};

} // !namespace system

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_LOGGER_H_

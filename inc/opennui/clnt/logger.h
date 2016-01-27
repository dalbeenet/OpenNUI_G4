#ifndef _OPENNUI_SDK4_CLNT_LOGGER_H_
#define _OPENNUI_SDK4_CLNT_LOGGER_H_

#include <string>

namespace opennui {

namespace clnt {

class logger abstract
{
public:
    static void system_log(std::string& s);
    static void system_log(const char* format, ...);
    static void system_error_log(std::string& s);
    static void system_error_log(const char* format, ...);
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SDK4_CLNT_LOGGER_H_

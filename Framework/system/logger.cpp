#include <system/logger.h>
#include <array>
#include <cstdarg>

namespace opennui {

namespace sys {

void logger::system_log(const char* format, ...)
{
    std::array<char, 2048> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buffer.data(), buffer.size(), format, ap);
    printf("log> %s\n", buffer.data());
    va_end(ap);
}

void logger::system_log(std::string& s)
{
    printf("log> %s\n", s.c_str());
}

void logger::system_error_log(const char* format, ...)
{
    std::array<char, 2048> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buffer.data(), buffer.size(), format, ap);
    printf("err> %s\n", buffer.data());
    va_end(ap);
}

void logger::system_error_log(std::string& s)
{
    printf("err> %s\n", s.c_str());
}

} // !namespace sys

} // !namespace opennui
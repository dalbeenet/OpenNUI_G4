#include <opennui/clnt/logger.h>
#include <array>
#include <cstdarg>
#include <vee/base.h>
#ifdef VEE_PLATFORM_WINDOWS
#include <Windows.h>
#endif // !VEE_PLATFORM_WINDOWS
#include <vee/concurrency/mutex.h>
#include <mutex>

namespace opennui {

namespace clnt {

::vee::spin_lock logger_lock;

#ifdef VEE_PLATFORM_WINDOWS
WORD get_console_textcolor()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE)
                               , &csbi);
    WORD r = csbi.wAttributes;
    return r % 16;
}

WORD get_console_bgcolor()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE)
                               , &csbi);
    WORD r = csbi.wAttributes;
    return (r / 16) % 16;
}

WORD set_console_color(WORD color)
{
    WORD old = (get_console_bgcolor() << 4) | get_console_textcolor();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
    SetConsoleTextAttribute(hConsole, color);
    return old;
}

::std::pair<WORD, WORD> set_console_color(WORD textcol, WORD backcol)
{
    ::std::pair<WORD, WORD> old;
    old.first = get_console_textcolor();
    old.second = get_console_bgcolor();
    textcol %= 16;
    backcol %= 16;
    unsigned short wAttributes = (backcol << 4) | textcol;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
    return old;
}

#endif // !VEE_PLATFORM_WINDOWS

void logger::system_log(const char* format, ...)
{
    std::array<char, 2048> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buffer.data(), buffer.size(), format, ap);
    {
        ::std::lock_guard<::vee::spin_lock> guard(logger_lock);
        fprintf(stdout, "log> %s\n", buffer.data());
    }
    va_end(ap);
}

void logger::system_log(std::string& s)
{
    ::std::lock_guard<::vee::spin_lock> guard(logger_lock);
    fprintf(stdout, "log> %s\n", s.c_str());
}

void logger::system_error_log(const char* format, ...)
{
    std::array<char, 2048> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buffer.data(), buffer.size(), format, ap);
    {
        ::std::lock_guard<::vee::spin_lock> guard(logger_lock);
        WORD old = set_console_color(12);
        fprintf(stdout, "err> %s\n", buffer.data());
        set_console_color(old);
    }
    va_end(ap);
}

void logger::system_error_log(std::string& s)
{
    ::std::lock_guard<::vee::spin_lock> guard(logger_lock);
    WORD old = set_console_color(12);
    fprintf(stdout, "err> %s\n", s.c_str());
    set_console_color(old);
}

void logger::system_msg(unsigned short color, const char* format, ...)
{
    std::array<char, 2048> buffer;
    buffer.fill(0);
    va_list ap;
    va_start(ap, format);
    vsprintf_s(buffer.data(), buffer.size(), format, ap);
    {
        ::std::lock_guard<::vee::spin_lock> guard(logger_lock);
        WORD old = set_console_color(color);
        fprintf(stdout, "sys> %s\n", buffer.data());
        set_console_color(old);
    }
    va_end(ap);
}

} // !namespace clnt

} // !namespace opennui
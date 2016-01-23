#include <iostream>
#include <system/device_manager.h>
#include <thread>
#include <chrono>

using namespace opennui;

int main()
{
    namespace sys = ::opennui::system;
    sys::device_loader::create_all();
    //::std::this_thread::sleep_for(::std::chrono::milliseconds::duration(2000));
    return 0;
}
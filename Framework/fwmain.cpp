#include <iostream>
#include <system/device_manager.h>
#include <system/comm/gateway.h>
#include <system/frdmgr.h>
#include <thread>
#include <chrono>
#include <conio.h>

//using namespace opennui;
#pragma warning(disable:4127)
#pragma warning(disable:4996)
int main()
{
    ::std::thread test_thr([]()
    {
        puts("TEST THR");
    });
    test_thr.join();
    system("color 7");
    namespace sys = ::opennui::sys;
    auto devices = sys::device_loader::create_all();
    sys::comm::gateway::open();
    sys::logger::system_msg(0x0E, "press enter key to exit...");
    while (true)
    {
        int ch = getch();
        if (ch == 13)
            break;
    }
    //sys::frdmgr::force_release_all_devices();
    /*for (auto& device : devices)
    {
        sys::frdmgr::send_message(sys::frdmgr::frdmgr_msg::option::remove, device);
    }*/
    sys::scheduler.force_clear_actors();
    return 0;
}
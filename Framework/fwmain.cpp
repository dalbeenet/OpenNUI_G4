#include <iostream>
#include <system/device_manager.h>
#include <system/comm/gateway.h>
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
    sys::device_loader::create_all();
    sys::comm::gateway::open();
    printf("press enter key to exit...\n");
    while (true)
    {
        int ch = getch();
        if (ch == 13)
            break;
    }
    sys::scheduler.force_clear_actors();
    return 0;
}
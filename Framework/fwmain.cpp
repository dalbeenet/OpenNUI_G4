#include <iostream>
#include <system/device_manager.h>
#include <system/comm/gateway.h>
#include <thread>
#include <chrono>

using namespace opennui;

int main()
{
    namespace sys = ::opennui::sys;
    system("color 7");
    sys::device_loader::create_all();
    sys::comm::gateway::open();
    {
        printf("press enter key to exit...\n");
        char buffer[128];
        fgets(buffer, 128, stdin);
    }
    return 0;
}
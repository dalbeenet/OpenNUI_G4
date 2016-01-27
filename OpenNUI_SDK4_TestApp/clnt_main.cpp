#include <opennui/opennui.h>
#include <opennui/clnt/comm/session.h>

int main()
{
    try
    {
        auto cs = ::opennui::clnt::comm::connect_to_local_framework();
        printf("press enter key to exit...\n");
        char buffer[128];
        fgets(buffer, 128, stdin);
    }
    catch (::vee::exception& e)
    {
        printf("err> %s\n", e.to_string());
    }
    return 0;
}
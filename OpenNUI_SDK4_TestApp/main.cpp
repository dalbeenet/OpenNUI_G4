#include <opennui/opennui.h>
#include <opennui/clnt/comm/session.h>

int main()
{
    try
    {
        auto cs = ::opennui::clnt::comm::connect_to_local_framework();
    }
    catch (::vee::exception& e)
    {
        printf("err> %s\n", e.to_string());
    }
    /*{
        printf("press enter key to exit...\n");
        char buffer[128];
        fgets(buffer, 128, stdin);
    }*/
    return 0;
}
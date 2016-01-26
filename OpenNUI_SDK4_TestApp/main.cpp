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
    return 0;
}
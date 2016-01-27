#include <opennui/opennui.h>
#include <opennui/clnt/comm/session.h>
#include <opennui/clnt/logger.h>
#include <list>
#include <conio.h>
#pragma warning(disable:4127)
#pragma warning(disable:4996)

int main()
{
    ::std::list<::opennui::clnt::comm::native_session::shared_ptr> sessions;
    try
    {
        while (true)
        {
            auto cs = ::opennui::clnt::comm::connect_to_local_framework();
            sessions.push_back(cs);
            printf("press enter key to create new client...\n");
            while (true)
            {
                int ch = getch();
                if (ch == 13)
                    break;
                else if (ch == 27)
                {
                    return 0;
                }
                else if (ch == 'c')
                {
                    sessions.clear();
                    continue;
                }
            }
        }
    }
    catch (::vee::exception& e)
    {
        printf("err> %s\n", e.to_string());
    }
    return 0;
}
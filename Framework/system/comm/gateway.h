#ifndef _OPENNUI_SYSTEM_COMM_GATEWAY_H_
#define _OPENNUI_SYSTEM_COMM_GATEWAY_H_

#include <system/exceptions.h>
#include <system/comm/protocol.h>
#include <vee/network/net.h>

namespace opennui {

namespace sys {

namespace comm {

class gateway final
{
public:
    using stream_t = ::vee::net::net_stream::shared_ptr;
    ~gateway();
    
private:
    static gateway& _get_instance();
    gateway();
    void _initialize();
    static void __stdcall _on_client_connected(::vee::net::op_result&/*operation result*/, ::vee::net::net_stream::shared_ptr/*stream*/);
    static void __stdcall _on_web_client_connected(::vee::net::op_result&/*operation result*/, ::vee::net::net_stream::shared_ptr/*stream*/);
    //static void __stdcall _on_client_data_received(stream_t stream, );
private:
    ::vee::net::net_server::shared_ptr _handshake_server;
    ::vee::net::net_server::shared_ptr _web_server;
};

} // !namespace comm

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_COMM_GATEWAY_H_

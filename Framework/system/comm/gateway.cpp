#include <system/comm/gateway.h>
#include <system/logger.h>

namespace opennui {

namespace sys {

namespace comm {

gateway::~gateway()
{

}

gateway& gateway::_get_instance()
{
    static gateway unique_instance;
    return unique_instance;
}

gateway::gateway()
{
    _initialize();
}

void gateway::_initialize()
{
    namespace net = ::vee::net;
    try
    {
        _handshake_server = net::tcp::create_server(protocol::comm_port::handshake_port);
        _web_server = net::rfc6455::create_server(protocol::comm_port::web_comm_port);
        _handshake_server->async_accept(_on_client_connected);
        _web_server->async_accept(_on_web_client_connected);
    }
    catch (::vee::exception& e)
    {
        sys::logger::system_error_log(e.to_string());
    }
}

void __stdcall gateway::_on_client_connected(::vee::net::op_result& operation_result, ::vee::net::net_stream::shared_ptr stream)
{
    if (operation_result.is_success)
    {
        
    }
    else
    {

    }
}

void __stdcall gateway::_on_web_client_connected(::vee::net::op_result& operation_result, ::vee::net::net_stream::shared_ptr stream)
{

}

void __stdcall gateway::_handshake(stream_t stream)
{

}

void __stdcall gateway::_web_handshake(stream_t stream)
{

}

} // !namespace comm

} // !namespace sys

} // !namespace opennui
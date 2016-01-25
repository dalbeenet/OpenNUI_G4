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
        _native_server = net::tcp::create_server(protocol::comm_port::handshake_port);
        _web_server = net::rfc6455::create_server(protocol::comm_port::web_comm_port);
        _native_server->async_accept(_on_client_connected);
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
        logger::system_log("new client connected, start the handshake process soon!");
        //TODO: Create session and call receive seq
    }
    else
    {
        logger::system_error_log("native server asynchronous connection failed!");
    }
}

void __stdcall gateway::_on_web_client_connected(::vee::net::op_result& operation_result, ::vee::net::net_stream::shared_ptr stream)
{
    if (operation_result.is_success)
    {
        logger::system_log("new web client connected, start the handshake process soon!");
        //TODO: Create session and call receive seq
    }
    else
    {
        logger::system_error_log("web server asynchronous connection failed!");
    }
}

void __stdcall gateway::_on_data_received(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    if (io_result.is_success)
    {
        switch (session->state.enum_form())
        {
        case session::state_t::read_header:
        {
            uint32_t remaining_header_size = protocol::message::header_size - session->bytes_transferred;
            if (io_result.bytes_transferred < remaining_header_size)
            {
                memmove(&session->message.header + session->bytes_transferred, buffer, io_result.bytes_transferred);
                session->bytes_transferred += io_result.bytes_transferred;
                session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
            }
            else
            {
                uint32_t excess = io_result.bytes_transferred - remaining_header_size;
                memmove(&session->message.header + session->bytes_transferred, buffer, remaining_header_size);
                if (excess > 0)
                    memmove(&session->message.data, buffer + remaining_header_size, excess);
                session->bytes_transferred += io_result.bytes_transferred;
                session->switching_state(session::state_t::read_data);
                session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
            }
            break;
        } // !case read_header
        case session::state_t::read_data:
        {

            break;
        } // !case read_data
        default:
        {

            logger::system_error_log("invalid receive mode");
            break;
        } // !case default
        } // !switch (session->state.enum_form()) 
    }
    else
    {
        if (io_result.eof)
        {
            if (io_result.bytes_transferred != 0)
            {
                logger::system_error_log("client disconneced, %d bytes of data lost", io_result.bytes_transferred);
            }
            else
            {
                logger::system_log("client disconnected!");
            }
        }
    }
}

} // !namespace comm

} // !namespace sys

} // !namespace opennui
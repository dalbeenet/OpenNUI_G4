#include <system/comm/gateway.h>
#include <system/logger.h>

namespace opennui {

namespace sys {

namespace comm {

session::session(stream_t _stream, uint32_t _id):
stream(_stream),
state(state_t::init),
id(_id),
bytes_transferred(0)
{
    
}

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
        _web_server->async_accept(_on_data_received);
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
        session::shared_ptr s = ::std::make_shared<session>(stream, _generate_sid());
        s->switching_state(session::state_t::read_header);
        stream->async_read_some(s->buffer.data(), s->buffer.size(), ::std::bind(_on_data_received, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        logger::system_error_log("native server asynchronous connection failed!");
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

uint32_t gateway::_generate_sid()
{
    static int sid = 100;
    return sid++;
}

} // !namespace comm

} // !namespace sys

} // !namespace opennui
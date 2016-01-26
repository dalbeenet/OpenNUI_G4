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
        _native_server = net::tcp::create_server(protocol::comm::comm_port::handshake_port);
        _web_server = net::rfc6455::create_server(protocol::comm::comm_port::web_comm_port);
        _native_server->async_accept(_on_client_connected);
        _web_server->async_accept(_on_client_connected);
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
        stream->async_read_some(s->buffer.data(), s->buffer.size(), ::std::bind(_on_data_received, s, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
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
            _header_processing(session, io_result, buffer, buffer_size);
            break;
        } // !case read_header
        case session::state_t::read_data:
        {
            _data_processing(session, io_result, buffer, buffer_size);
            break;
        } // !case read_data
        default:
        {
            logger::system_error_log("invalid state! (sid: %d)", session->id);
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
                logger::system_error_log("client(sid: %d) disconnected, %d bytes of data lost", session->id, io_result.bytes_transferred);
            }
            else
            {
                logger::system_log("client(sid: %d) disconnected!", session->id);
            }
        }
    }
}

void __stdcall gateway::_header_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    size_t remaining_header_size = sizeof(protocol::comm::message_header) - session->bytes_transferred;
    if (io_result.bytes_transferred < remaining_header_size)
    {
        memmove(&session->message.header + session->bytes_transferred, buffer, io_result.bytes_transferred);
        session->bytes_transferred += io_result.bytes_transferred;
        session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        size_t excess_quantity = io_result.bytes_transferred - remaining_header_size;
        memmove(&session->message.header + session->bytes_transferred, buffer, remaining_header_size);
        session->bytes_transferred += remaining_header_size;
        try
        {
            protocol::comm::validate_header(session->message.header);
        }
        catch (::vee::exception& e)
        {
            logger::system_error_log("The client(sid: %d) has sent an abnormal message header!\n\tdetail: %s", session->id, e.to_string());
            session->clear_buffer();
            if (excess_quantity > 0)
            {
                io_result.bytes_transferred = excess_quantity;
                memmove(buffer, buffer + remaining_header_size, excess_quantity);
                return _header_processing(session, io_result, buffer, buffer_size);
            }
        }
        session->switching_state(session::state_t::read_data);
        if (excess_quantity > 0)
        {
            io_result.bytes_transferred = excess_quantity;
            memmove(buffer, buffer + remaining_header_size, excess_quantity);
            _data_processing(session, io_result, buffer, buffer_size);
        }
        else
        {
            session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
        }
        return;
        ///*if (excess_quantity > 0)
        //    memmove(&session->message.data, buffer + remaining_header_size, excess_quantity);
        //    session->bytes_transferred += io_result.bytes_transferred;
    }
}

void __stdcall gateway::_data_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    size_t data_transferred = session->bytes_transferred - sizeof(protocol::comm::message_header);
    size_t remaining_data_size = session->message.header.block_size - data_transferred;
    if (io_result.bytes_transferred < remaining_data_size)
    {
        memmove(&session->message.data + data_transferred, buffer, io_result.bytes_transferred);
        session->bytes_transferred += io_result.bytes_transferred;
        session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        size_t excess_quantity = io_result.bytes_transferred - remaining_data_size;
        memmove(&session->message.data, buffer, remaining_data_size);
        session->bytes_transferred += remaining_data_size;
        excess_quantity -= remaining_data_size;

        logger::system_log("client(sid: %d) message arrived\n\topcode: %d\n\tblock size: %d\n\tmessage id: %d",
                           session->id,
                           session->message.header.opcode.to_int32(),
                           session->message.header.block_size,
                           session->message.header.message_id);
        //TODO: GOTO -> Query Process (pass the message via value-copy)

        if (excess_quantity > 0)
        {
            io_result.bytes_transferred = excess_quantity;
            memmove(buffer, buffer + remaining_data_size, excess_quantity);
            session->clear_buffer();
            session->switching_state(session::state_t::read_header);
            _header_processing(session, io_result, buffer, buffer_size);
        }
        return;
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
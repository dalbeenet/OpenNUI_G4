#include <system/comm/gateway.h>
#include <system/logger.h>

namespace opennui {

namespace sys {

namespace comm {

session::session(stream_t _stream, uint32_t _id):
stream(_stream),
state(state_t::init),
type(clnttype::null),
id(_id),
bytes_transferred_in(0)
{
    buffer_in.fill(0);
    buffer_out.fill(0);
}

session::session(session&& other):
stream(::std::move(other.stream)),
state(::std::move(other.state)),
type(::std::move(other.type)),
id(::std::move(other.id)),
bytes_transferred_in(::std::move(other.bytes_transferred_in)),
buffer_in(::std::move(other.buffer_in)),
buffer_out(::std::move(other.buffer_out))
{

}

session& session::operator=(session&& rhs)
{
    stream = ::std::move(rhs.stream);
    state = ::std::move(rhs.state);
    type = ::std::move(rhs.type);
    id = ::std::move(rhs.id);
    bytes_transferred_in = ::std::move(rhs.bytes_transferred_in);
    buffer_in = ::std::move(rhs.buffer_in);
    buffer_out = ::std::move(rhs.buffer_out);
    return *this;
}

session::~session()
{
    logger::system_log("session %d destroyed", id);
}

gateway::~gateway()
{
    close();
    logger::system_log("gateway destroyed");
}

gateway& gateway::_get_instance()
{
    static gateway unique_instance;
    return unique_instance;
}

gateway::gateway():
_flag_open(false)
{
    // open();
}

void __stdcall gateway::open()
{
    namespace net = ::vee::net;
    gateway& inst = _get_instance();
    try
    {
        gateway::close();
        inst._native_server = net::tcp::create_server(protocol::comm::comm_port::native_comm_port);
        inst._web_server = net::rfc6455::create_server(protocol::comm::comm_port::web_comm_port);
        inst._native_server->async_accept(::std::bind(_on_client_connected, inst._native_server, server_type::native, ::std::placeholders::_1, ::std::placeholders::_2));
        logger::system_log("native server is online");
        inst._web_server->async_accept(::std::bind(_on_client_connected, inst._web_server, server_type::web, ::std::placeholders::_1, ::std::placeholders::_2));
        logger::system_log("web server is online");
        inst._flag_open = true;
    }
    catch (::vee::exception& e)
    {
        sys::logger::system_error_log(e.to_string());
    }
}

void __stdcall gateway::close()
{
    gateway& inst = _get_instance();
    if (inst._native_server.use_count() != 0)
    {
        inst._native_server->close();
        inst._native_server.reset();
    }
    if (inst._web_server.use_count() != 0)
    {
        inst._web_server->close();
        inst._web_server.reset();
    }
    inst._flag_open = false;
}

void __stdcall gateway::_on_client_connected(server_t server, server_type type,::vee::net::op_result& operation_result, ::vee::net::net_stream::shared_ptr stream)
{
    if (operation_result.is_success)
    {
        session::shared_ptr s = ::std::make_shared<session>(stream, _generate_sid());
        logger::system_log("new %s client(sid: %d) connected, start the handshake process soon!", type.to_string(), s->id);
        s->switching_state(session::state_t::read_header);
        stream->async_read_some(s->buffer_in.data(), s->buffer_in.size(), ::std::bind(_on_data_received, s, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
        server->async_accept(::std::bind(_on_client_connected, server, type, ::std::placeholders::_1, ::std::placeholders::_2));
    }
    else
    {
        logger::system_error_log("%s server asynchronous accept failed!", type.to_string());
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
                logger::system_error_log("client(sid: %d) disconnected, %d bytes of data loss", session->id, io_result.bytes_transferred);
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
    size_t remaining_header_size = sizeof(protocol::comm::message_header) - session->bytes_transferred_in;
    if (io_result.bytes_transferred < remaining_header_size)
    {
        memmove(&session->msgbuf_in.header + session->bytes_transferred_in, buffer, io_result.bytes_transferred);
        session->bytes_transferred_in += io_result.bytes_transferred;
        session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        size_t excess_quantity = io_result.bytes_transferred - remaining_header_size;
        memmove(&session->msgbuf_in.header + session->bytes_transferred_in, buffer, remaining_header_size);
        session->bytes_transferred_in += remaining_header_size;
        try
        {
            protocol::comm::validate_header(session->msgbuf_in.header);
        }
        catch (::vee::exception& e)
        {
            logger::system_error_log("The client(sid: %d) has sent an abnormal message header!\n\tdetail: %s", session->id, e.to_string());
            session->clear_msgbuf_in();
            if (excess_quantity > 0)
            {
                io_result.bytes_transferred = excess_quantity;
                memmove(buffer, buffer + remaining_header_size, excess_quantity);
                return _header_processing(session, io_result, buffer, buffer_size);
            }
        }
        session->switching_state(session::state_t::read_data);
        if (excess_quantity >= 0)
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
    }
}

void __stdcall gateway::_data_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    size_t data_transferred = session->bytes_transferred_in - sizeof(protocol::comm::message_header);
    size_t remaining_data_size = session->msgbuf_in.header.block_size - data_transferred;
    if (io_result.bytes_transferred < remaining_data_size)
    {
        memmove(&session->msgbuf_in.data + data_transferred, buffer, io_result.bytes_transferred);
        session->bytes_transferred_in += io_result.bytes_transferred;
        session->stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        size_t excess_quantity = io_result.bytes_transferred - remaining_data_size;
        memmove(&session->msgbuf_in.data, buffer, remaining_data_size);
        session->bytes_transferred_in += remaining_data_size;
        excess_quantity -= remaining_data_size;

        logger::system_log("client(sid: %d) message arrived\n\topcode: %s\n\tblock size: %d\n\tmessage id: %d",
                           session->id,
                           session->msgbuf_in.header.opcode.to_string(),
                           session->msgbuf_in.header.block_size,
                           session->msgbuf_in.header.message_id);
        
        _query_processing(session);

        if (excess_quantity >= 0)
        {
            io_result.bytes_transferred = excess_quantity;
            memmove(buffer, buffer + remaining_data_size, excess_quantity);
            session->clear_msgbuf_in();
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

void __stdcall gateway::_query_processing(session::shared_ptr& session)
{
    protocol::comm::message_header& header = session->msgbuf_in.header;
    unsigned char* data = session->msgbuf_in.data.data();
    logger::system_log("Enter query processing: sid: %d, opcode: %d", session->id, header.opcode);

}

} // !namespace comm

} // !namespace sys

} // !namespace opennui
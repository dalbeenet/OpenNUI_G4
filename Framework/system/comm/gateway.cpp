#include <opennui/exceptions.h>
#include <system/sysbase.h>
#include <system/comm/gateway.h>
#include <system/logger.h>
#include <vee/interprocess/pipe.h>
#include <thread>
#include <future>

namespace opennui {

namespace sys {

namespace comm {

session::session(stream_t _stream, uint32_t _id):
keep_alive_stream(_stream),
state(state_t::init),
type(protocol::comm::client_type::null),
id(_id),
bytes_transferred_in(0)
{
    buffer_in.fill(0);
    buffer_out.fill(0);
}

session::session(session&& other):
keep_alive_stream(::std::move(other.keep_alive_stream)),
cts_msg_stream(::std::move(other.cts_msg_stream)),
stc_msg_stream(::std::move(other.stc_msg_stream)),
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
    keep_alive_stream = ::std::move(rhs.keep_alive_stream);
    cts_msg_stream = ::std::move(rhs.cts_msg_stream);
    stc_msg_stream = ::std::move(rhs.stc_msg_stream);
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
        logger::system_log("Native server is online");
        inst._web_server->async_accept(::std::bind(_on_client_connected, inst._web_server, server_type::web, ::std::placeholders::_1, ::std::placeholders::_2));
        logger::system_log("Web server is online");
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
        session::shared_ptr s = ::std::make_shared<session>(::std::static_pointer_cast<::vee::io::stream>(stream), _generate_sid());
        logger::system_log("The new %s client(sid: %d) connected, start the handshake process soon!", type.to_string(), s->id);
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
            logger::system_error_log("Invalid state! (sid: %d)", session->id);
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
                logger::system_error_log("Stream read failed, client(sid: %d) disconnected, %d bytes of data loss", session->id, io_result.bytes_transferred);
            }
            else
            {
                logger::system_error_log("Stream read failed, client(sid: %d) disconnected!", session->id);
            }
        }
    }
}

void __stdcall gateway::_on_data_sent(session::shared_ptr session, ::vee::io::io_result& io_result)
{
    if (io_result.is_success)
    {
        logger::system_log("Message sent to client(sid: %d) \n\topcode: %s\n\tblock size: %d\n\tmessage id: %d",
                           session->id,
                           session->msgbuf_out.header.opcode.to_string(),
                           session->msgbuf_out.header.block_size,
                           session->msgbuf_out.header.message_id);
    }
    else
    {
        if (io_result.eof)
        {
            if (io_result.bytes_transferred != 0)
            {
                logger::system_error_log("Stream write failed, client(sid: %d) disconnected, %d bytes of data loss", session->id, io_result.bytes_transferred);
            }
            else
            {
                logger::system_error_log("Stream write failed, client(sid: %d) disconnected!", session->id);
            }
        }
    }
}

void __stdcall gateway::_header_processing(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    size_t remaining_header_size = sizeof(protocol::comm::message_header) - session->bytes_transferred_in;
    if (io_result.bytes_transferred < remaining_header_size)
    {
        memmove(&session->msgbuf_in.header + session->bytes_transferred_in, buffer, io_result.bytes_transferred);
        session->bytes_transferred_in += io_result.bytes_transferred;
        session->keep_alive_stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
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
            session->keep_alive_stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
        }
        return;
    }
}

void __stdcall gateway::_data_processing(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size)
{
    size_t data_transferred = session->bytes_transferred_in - sizeof(protocol::comm::message_header);
    size_t remaining_data_size = session->msgbuf_in.header.block_size - data_transferred;
    if (io_result.bytes_transferred < remaining_data_size)
    {
        memmove(&session->msgbuf_in.data_section + data_transferred, buffer, io_result.bytes_transferred);
        session->bytes_transferred_in += io_result.bytes_transferred;
        session->keep_alive_stream->async_read_some(buffer, buffer_size, ::std::bind(_on_data_received, session, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3));
    }
    else
    {
        size_t excess_quantity = io_result.bytes_transferred - remaining_data_size;
        memmove(&session->msgbuf_in.data_section, buffer, remaining_data_size);
        session->bytes_transferred_in += remaining_data_size;

        logger::system_log("Client(sid: %d) message arrived\n\topcode: %s\n\tblock size: %d\n\tmessage id: %d",
                           session->id,
                           session->msgbuf_in.header.opcode.to_string(),
                           session->msgbuf_in.header.block_size,
                           session->msgbuf_in.header.message_id);
       
        try
        {
            _async_query_proc_launcher(session);
        }
        catch (::vee::exception& e)
        {
            logger::system_error_log("An error occured to Launch async query processing\n\tdetail: %s", e.to_string());
        }

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

void __stdcall gateway::_async_query_proc_launcher(session::shared_ptr session) throw(...)
{
    using namespace protocol::comm;
    message_header& header_in = session->msgbuf_in.header;
    //unsigned char* data_in = session->msgbuf_in.data.data();
    
    switch (header_in.opcode.enum_form())
    {
    case opcode_t::handshake_hello:
    {
        ::std::thread handshake_thread(gateway::query_processing::handshake, session, session->msgbuf_in);
        handshake_thread.detach();
        //scheduler.request(::vee::make_delegate<OPENNUI_SYSTEM_TASK_SIG>(::std::bind(query_processing::handshake, session, session->msgbuf_in)));
        /*auto thread_launcher = [](session::shared_ptr session, protocol::comm::message copied_msg)->void
        {
        logger::system_log("Launch handshake thread!");
        ::std::thread handshake_thread(gateway::query_processing::handshake, session, copied_msg);
        handshake_thread.detach();
        };*/
        //scheduler.request(::vee::make_delegate<OPENNUI_SYSTEM_TASK_SIG>(::std::bind(thread_launcher, session, session->msgbuf_in)));
        break;
    }
    //case protocol::comm::opcode_t::
    default:
        throw protocol::exceptions::invalid_opcode(header_in.opcode.to_uint32());
    }
}

void __stdcall gateway::query_processing::handshake(session::shared_ptr session, protocol::comm::message copied_msg) __noexcept
{
    using namespace protocol::comm;
    message_header& header_in = copied_msg.header;
    unsigned char* data_in = copied_msg.data_section.data();
    message_header& header_out = session->msgbuf_out.header;
    unsigned char* data_out = session->msgbuf_out.data_section.data();

    // Parse the data from client
    if (header_in.block_size < sizeof(uint32_t))
    {
        auto e = protocol::exceptions::block_size_too_small(header_in.block_size);
        logger::system_error_log("An error occuerd in the %s\t\ndetail: %s", __FUNCTION__, e.to_string());
    }

    client_type clnt_t = client_type::null;
    memmove(&clnt_t, data_in, sizeof(client_type));
    switch (clnt_t.enum_form())
    {
    case client_type::native:
    case client_type::web:
        session->type = clnt_t;
        break;
    default:
    {
        auto e = protocol::exceptions::unsupported_client_type(clnt_t.to_int32());
        logger::system_error_log("An error occuerd in the %s\t\ndetail: %s", __FUNCTION__, e.to_string());
    }
    }

    // Do the spadework for message stream connection
    if (session->type == client_type::native)
    {
        auto cts_pipe_server = vee::interprocess::windows::create_named_pipe_server();
        auto stc_pipe_server = vee::interprocess::windows::create_named_pipe_server();
        using data_transfer_mode = vee::interprocess::named_pipe::data_transfer_mode;
        char pipe_name_base[512] = { 0, };
        sprintf_s(pipe_name_base, "\\\\.\\pipe\\opennui_g4_msgpipe_%d", session->id);

        session::stream_t cts_msg_stream;
        session::stream_t stc_msg_stream;

        // Launch async task
        auto cts_connection = std::async(std::launch::async, [&]() -> bool // CTS connection
        {
            std::string cts_pipe_name(pipe_name_base);
            cts_pipe_name.append("_cts");
            logger::system_log("Start the [client -> framework] message stream accept process\n\tsid: %d, pipe name: %s", session->id, cts_pipe_name.c_str());
            try
            {
                cts_msg_stream = cts_pipe_server->accept(cts_pipe_name.c_str(), data_transfer_mode::iomode_message, protocol::comm::pipe_io_buffer_size, protocol::comm::pipe_io_buffer_size, protocol::comm::pipe_server_timeout);
                logger::system_log("Succeed to [client -> framework] message stream accept process\n\tsid: %d, pipe name: %s", session->id, cts_pipe_name.c_str());
                return true;
            }
            catch (::vee::exception& e)
            {
                logger::system_error_log("An error occured from [client -> framework] message stream accept process\n\tsid: %d, pipe name: %s\n\tdetail: %s", session->id, cts_pipe_name.c_str(), e.to_string());
                return false;
            }
        });

        auto stc_connection = std::async(std::launch::async, [&]() -> bool // STC connection
        {
            std::string stc_pipe_name(pipe_name_base);
            stc_pipe_name.append("_stc");
            logger::system_log("Start the [framework -> client] message stream accept process\n\tsid: %d, pipe name: %s", session->id, stc_pipe_name.c_str());
            try
            {
                stc_msg_stream = stc_pipe_server->accept(stc_pipe_name.c_str(), data_transfer_mode::iomode_message, protocol::comm::pipe_io_buffer_size, protocol::comm::pipe_io_buffer_size, protocol::comm::pipe_server_timeout);
                logger::system_log("Succeed to [framework -> client] message stream accept process\n\tsid: %d, pipe name: %s", session->id, stc_pipe_name.c_str());
                return true;
            }
            catch (::vee::exception& e)
            {
                logger::system_error_log("An error occured from [framework -> client] message stream accept process\n\tsid: %d, pipe name: %s\n\tdetail: %s", session->id, stc_pipe_name.c_str(), e.to_string());
                return false;
            }
        });

        ::std::this_thread::sleep_for(::std::chrono::milliseconds::duration(1000));

        {
            header_out.opcode = opcode_t::handshake_ack;
            header_out.block_size = sizeof(session->id);
            header_out.message_id = 1;
            memmove(data_out, &session->id, sizeof(session->id));
            uint32_t szmsg = session->msgbuf_out.to_binary(session->buffer_out.data());
            session->keep_alive_stream->async_write_some(session->buffer_out.data(), szmsg, ::std::bind(_on_data_sent, session, ::std::placeholders::_1));
        }

        bool cts_result = cts_connection.get();
        bool stc_result = stc_connection.get();
        if (!cts_result || !stc_result)
        {
            logger::system_error_log("Message stream connection failed(sid: %d), cts: %s / stc: %s",
                                                         session->id, 
                                                         (cts_result ? "OK" : "NA"),
                                                         (stc_result ? "OK" : "NA"));
            logger::system_error_log("Handshake failed! sid: %d", session->id);
            return;
        }

        session->cts_msg_stream = cts_msg_stream;
        session->stc_msg_stream = stc_msg_stream;
        logger::system_log("Handshake success! sid: %d", session->id);
        return;
    }
    else if (session->type == client_type::web)
    {
        logger::system_error_log("Unsupported client type (TODO)");
    }
    else
    {
        logger::system_error_log("Bad error: invalid client type");
    }
}

} // !namespace comm

} // !namespace sys

} // !namespace opennui
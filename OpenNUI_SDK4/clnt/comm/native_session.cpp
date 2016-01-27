#include <opennui/clnt/comm/session.h>
#include <opennui/clnt/logger.h>
#include <vee/interprocess/pipe.h>
#include <Windows.h>

namespace opennui {

namespace clnt {

namespace comm {

native_session::native_session(stream_t _stream):
keep_alive_stream(_stream),
id(NULL)
{
    
}

#pragma warning(disable:4127)
protocol::comm::message read_message(native_session::shared_ptr ns)
{
    using namespace protocol::comm;
    
    {
        size_t bytes_transferred = 0;
        ns->clear_msgbuf_in();
        while (true)
        {
            bytes_transferred += ns->keep_alive_stream->read_some(ns->buffer_in.data() + bytes_transferred, sizeof(message_header) - bytes_transferred);
            if (bytes_transferred == sizeof(message_header))
                break;
            else if (bytes_transferred > sizeof(message_header))
            {
                logger::system_error_log("Bad error: read too many bytes");
                throw ::std::runtime_error("Bad error: read too many bytes in the opennui::clnt::comm::read_message()");
            }
        }

        memmove(&ns->msgbuf_in.header, ns->buffer_in.data(), sizeof(message_header));
        logger::system_log("client(sid: %d) message arrived\n\topcode: %s\n\tblock size: %d\n\tmessage id: %d",
                           ns->id,
                           ns->msgbuf_in.header.opcode.to_string(),
                           ns->msgbuf_in.header.block_size,
                           ns->msgbuf_in.header.message_id);
    }

    while (true)
    {
        size_t bytes_transferred = 0;
        if (ns->msgbuf_in.header.block_size)
        {
            bytes_transferred += ns->keep_alive_stream->read_some(ns->buffer_in.data() + bytes_transferred, ns->msgbuf_in.header.block_size - bytes_transferred);
            if (bytes_transferred == sizeof(ns->msgbuf_in.header.block_size))
                break;
            else if (bytes_transferred > sizeof(ns->msgbuf_in.header.block_size))
            {
                logger::system_error_log("Bad error: read too many bytes");
                throw ::std::runtime_error("Bad error: read too many bytes in the opennui::clnt::comm::read_message()");
            }
        }
    }

    memmove(ns->msgbuf_in.data_section.data(), ns->buffer_in.data(), ns->msgbuf_in.header.block_size);
    logger::system_log("client(sid: %d) message data arrived", ns->id);

    return ns->msgbuf_in;
}
#pragma warning(default:4127)

native_session::shared_ptr connect_to_local_framework() throw(...)
{
    using namespace protocol::comm;

    auto stream = ::vee::net::tcp::create_stream();
    stream->connect("127.0.0.1", comm_port::native_comm_port);
    native_session::shared_ptr ns = ::std::make_shared<native_session>(::std::static_pointer_cast<::vee::io::stream>(stream));
    logger::system_log("Keep-alive stream attached to framework");

    auto& out_header = ns->msgbuf_out.header;
    unsigned char* out_data = ns->msgbuf_out.data_section.data();

    out_header.opcode = opcode_t::handshake_hello;
    out_header.block_size = sizeof(client_type);
    out_header.message_id = 1;
    client_type clnt_t = client_type::native;
    memmove(out_data, &clnt_t, sizeof(client_type));
    uint32_t szmsg = ns->msgbuf_out.to_binary(ns->buffer_out.data());
    ns->keep_alive_stream->write_some(ns->buffer_out.data(), szmsg);

    {
        // handshake ack signal, framework sends a session id
        message msg = read_message(ns);
        memmove(&ns->id, msg.data_section.data(), sizeof(uint32_t));
        try
        {
            namespace intprc = ::vee::interprocess;
            intprc::named_pipe::shared_ptr cts_pipe = intprc::windows::create_named_pipe();
            intprc::named_pipe::shared_ptr stc_pipe = intprc::windows::create_named_pipe();
            
            char pipe_name_base[512] = { 0, };
            sprintf_s(pipe_name_base, "\\\\.\\pipe\\opennui_g4_msgpipe_%d", ns->id);
            
            std::string cts_pipe_name(pipe_name_base);
            cts_pipe_name.append("_cts");

            std::string stc_pipe_name(pipe_name_base);
            stc_pipe_name.append("_stc");

            logger::system_log("Start the [client -> framework] message stream accept process\n\tsid: %d, pipe name: %s", ns->id, cts_pipe_name.c_str());
            cts_pipe->connect(cts_pipe_name.c_str(), intprc::named_pipe::create_option::open_existing, intprc::named_pipe::data_transfer_mode::iomode_message, protocol::comm::pipe_client_timeout);
            logger::system_log("Succeed to [client -> framework] message stream accept process\n\tsid: %d, pipe name: %s", ns->id, cts_pipe_name.c_str());

            logger::system_log("Start the [framework -> client] message stream accept process\n\tsid: %d, pipe name: %s", ns->id, stc_pipe_name.c_str());
            stc_pipe->connect(stc_pipe_name.c_str(), intprc::named_pipe::create_option::open_existing, intprc::named_pipe::data_transfer_mode::iomode_message, protocol::comm::pipe_client_timeout);
            logger::system_log("Succeed to [framework -> client] message stream accept process\n\tsid: %d, pipe name: %s", ns->id, stc_pipe_name.c_str());

            ns->cts_msg_stream = ::std::static_pointer_cast<::vee::io::stream>(cts_pipe);
            ns->stc_msg_stream = ::std::static_pointer_cast<::vee::io::stream>(stc_pipe);
        }
        catch (::vee::exception& e)
        {
            logger::system_error_log("An error occured to establish message connections\n\tdetail: %s", e.to_string());
        }
    }

    return ns;
}

} // !namespace comm

} // !namespace clnt

} // !namespace opennui
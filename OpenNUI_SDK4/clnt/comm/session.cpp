#include <opennui/clnt/comm/session.h>
#include <opennui/clnt/logger.h>

namespace opennui {

namespace clnt {

namespace comm {

native_session::native_session(stream_t _stream):
stream(_stream),
id(NULL)
{
    
}

#pragma warning(disable:4127)
void read_message(native_session::shared_ptr ns)
{
    using namespace protocol::comm;
    
    {
        size_t bytes_transferred = 0;
        ns->clear_msgbuf_in();
        while (true)
        {
            bytes_transferred += ns->stream->read_some(ns->buffer_in.data() + bytes_transferred, sizeof(message_header) - bytes_transferred);
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
            bytes_transferred += ns->stream->read_some(ns->buffer_in.data() + bytes_transferred, ns->msgbuf_in.header.block_size - bytes_transferred);
            if (bytes_transferred == sizeof(ns->msgbuf_in.header.block_size))
                break;
            else if (bytes_transferred > sizeof(ns->msgbuf_in.header.block_size))
            {
                logger::system_error_log("Bad error: read too many bytes");
                throw ::std::runtime_error("Bad error: read too many bytes in the opennui::clnt::comm::read_message()");
            }
        }
    }

    memmove(ns->msgbuf_in.data.data(), ns->buffer_in.data(), ns->msgbuf_in.header.block_size);
    logger::system_log("client(sid: %d) message data arrived", ns->id);
}
#pragma warning(default:4127)

native_session::shared_ptr connect_to_local_framework() throw(...)
{
    using namespace protocol::comm;

    auto stream = ::vee::net::tcp::create_stream();
    stream->connect("127.0.0.1", comm_port::native_comm_port);
    native_session::shared_ptr ns = ::std::make_shared<native_session>(stream);
    logger::system_log("Keep-alive stream attached to framework");

    auto& out_header = ns->msgbuf_out.header;
    unsigned char* out_data = ns->msgbuf_out.data.data();

    out_header.opcode = opcode_t::handshake_hello;
    out_header.block_size = sizeof(client_type);
    out_header.message_id = 1;
    client_type clnt_t = client_type::native;
    memmove(out_data, &clnt_t, sizeof(client_type));
    uint32_t szmsg = ns->msgbuf_out.to_binary(ns->buffer_out.data());
    ns->stream->write_some(ns->buffer_out.data(), szmsg);

    read_message(ns);

    return ns;
}

//client_session::unqiue_ptr connect_to_remote_framework()
//{
//
//}

} // !namespace comm

} // !namespace clnt

} // !namespace opennui
#include <opennui/clnt/comm/session.h>

namespace opennui {

namespace clnt {

namespace comm {

session::session(stream_t _stream):
stream(_stream),
id(NULL),
bytes_transferred_in(0)
{
    
}

session::shared_ptr connect_to_local_framework() throw(...)
{
    using namespace protocol::comm;

    auto stream = ::vee::net::tcp::create_stream();
    stream->connect("127.0.0.1", comm_port::native_comm_port);
    session::shared_ptr cs = ::std::make_shared<session>(stream);

    auto& header = cs->msgbuf_out.header;
    unsigned char* data = cs->msgbuf_out.data.data();

    header.opcode = opcode_t::handshake_hello;
    header.block_size = sizeof(uint32_t);
    header.message_id = 1;
    uint32_t clnt_t = static_cast<uint32_t>(client_type::native);
    memmove(cs->msgbuf_out.data.data(), &clnt_t, sizeof(uint32_t));
    uint32_t szmsg = cs->msgbuf_out.to_binary(cs->buffer_out.data());
    cs->stream->write_some(cs->buffer_out.data(), szmsg);
    return cs;
}

//client_session::unqiue_ptr connect_to_remote_framework()
//{
//
//}

} // !namespace comm

} // !namespace clnt

} // !namespace opennui
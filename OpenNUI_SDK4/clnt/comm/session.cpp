#include <opennui/clnt/comm/session.h>
#include <Windows.h>

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
    auto stream = ::vee::net::tcp::create_stream();
    stream->connect("127.0.0.1", protocol::comm::comm_port::native_comm_port);
    session::shared_ptr cs = ::std::make_shared<session>(stream);
    cs->msgbuf_out.header.opcode = protocol::comm::opcode_t::handshake_hello;
    cs->msgbuf_out.header.block_size = 0;
    cs->msgbuf_out.header.message_id = 1;
    uint32_t szmsg = cs->msgbuf_out.to_binary(cs->buffer_out.data());
    for (uint32_t i = 0; i < szmsg; ++i)
    {
        cs->stream->write_some(cs->buffer_out.data() + i, 1);
        Sleep(100);
    }
    return cs;
}

//client_session::unqiue_ptr connect_to_remote_framework()
//{
//
//}

} // !namespace comm

} // !namespace clnt

} // !namespace opennui
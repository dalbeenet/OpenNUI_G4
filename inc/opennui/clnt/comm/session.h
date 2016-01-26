#ifndef _OPENNUI_SDK4_CLNT_COMM_SESSION_H_
#define _OPENNUI_SDK4_CLNT_COMM_SESSION_H_

#include <opennui/protocol.h>
#include <vee/network/net.h>

namespace opennui {

namespace clnt {

namespace comm {

class session
{
    DISALLOW_COPY_AND_ASSIGN(session);
    DISALLOW_MOVE_AND_ASSIGN(session);
    session() = delete;
public:
    using shared_ptr = ::std::shared_ptr<session>;
    using unqiue_ptr = ::std::unique_ptr<session>;
    using stream_t = ::vee::net::net_stream::shared_ptr;
    session(stream_t _stream);
    inline void clear_msgbuf_in()
    {
        bytes_transferred_in = 0;
        msgbuf_in.clear();
    }
    inline void clear_msgbuf_out()
    {
        msgbuf_out.clear();
    }

public:
    stream_t stream;
    uint32_t id;
    protocol::comm::message msgbuf_in;
    protocol::comm::message msgbuf_out;
    size_t   bytes_transferred_in;
    ::std::array<unsigned char, sizeof(protocol::comm::message)> buffer_in;
    ::std::array<unsigned char, sizeof(protocol::comm::message)> buffer_out;
};

session::shared_ptr connect_to_local_framework() throw(...);

} // !namespace comm

} // !namespace clnt

} // !namespace opennui

#endif // !_OPENNUI_SDK4_CLNT_COMM_SESSION_H_

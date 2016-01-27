#ifndef _OPENNUI_SDK4_CLNT_COMM_SESSION_H_
#define _OPENNUI_SDK4_CLNT_COMM_SESSION_H_

#include <opennui/protocol.h>
#include <vee/network/net.h>

namespace opennui {

namespace clnt {

namespace comm {

class native_session
{
    DISALLOW_COPY_AND_ASSIGN(native_session);
    DISALLOW_MOVE_AND_ASSIGN(native_session);
    native_session() = delete;
public:
    using shared_ptr = ::std::shared_ptr<native_session>;
    using unqiue_ptr = ::std::unique_ptr<native_session>;
    using stream_t = ::std::shared_ptr<::vee::io::stream>;
    native_session(stream_t _stream);
    inline void clear_msgbuf_in()
    {
        msgbuf_in.clear();
    }
    inline void clear_msgbuf_out()
    {
        msgbuf_out.clear();
    }

public:
    stream_t keep_alive_stream;
    stream_t cts_msg_stream;
    stream_t stc_msg_stream;
    uint32_t id;
    protocol::comm::message msgbuf_in;
    protocol::comm::message msgbuf_out;
    ::std::array<unsigned char, sizeof(protocol::comm::message)> buffer_in;
    ::std::array<unsigned char, sizeof(protocol::comm::message)> buffer_out;
};

native_session::shared_ptr connect_to_local_framework() throw(...);

} // !namespace comm

} // !namespace clnt

} // !namespace opennui

#endif // !_OPENNUI_SDK4_CLNT_COMM_SESSION_H_

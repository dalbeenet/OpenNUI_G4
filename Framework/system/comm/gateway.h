#ifndef _OPENNUI_SYSTEM_COMM_GATEWAY_H_
#define _OPENNUI_SYSTEM_COMM_GATEWAY_H_

#include <system/exceptions.h>
#include <system/comm/protocol.h>
#include <vee/network/net.h>

namespace opennui {

namespace sys {

namespace comm {

class session
{
    DISALLOW_COPY_AND_ASSIGN(session);
public:
    using shared_ptr = ::std::shared_ptr<session>;
    using unqiue_ptr = ::std::unique_ptr<session>;
    Enumeration(state_t, 0,
                init,
                read_header,
                read_data);
    using stream_t = ::vee::net::net_stream::shared_ptr;
    stream_t stream {nullptr};
    state_t  state = state_t::init;
    uint32_t id;
    protocol::message message;
    size_t   bytes_transferred = 0;
    inline void switching_state(state_t new_state)
    {
        state = new_state;
    }
    inline void clear_buffer()
    {
        bytes_transferred = 0;
        message.clear();
    }
};

class gateway final
{
public:
    ~gateway();
    
private:
    
    static gateway& _get_instance();
    gateway();
    void _initialize();
    static void __stdcall _on_client_connected(::vee::net::op_result&/*operation result*/, ::vee::net::net_stream::shared_ptr/*stream*/);
    static void __stdcall _on_web_client_connected(::vee::net::op_result&/*operation result*/, ::vee::net::net_stream::shared_ptr/*stream*/);
    static void __stdcall _on_data_received(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const recieve_buffer_address, size_t recieve_buffer_size);
private:
    ::vee::net::net_server::shared_ptr _native_server;
    ::vee::net::net_server::shared_ptr _web_server;
};

} // !namespace comm

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_COMM_GATEWAY_H_

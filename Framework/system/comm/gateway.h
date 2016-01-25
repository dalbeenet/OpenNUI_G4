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
    using stream_t = ::vee::net::net_stream::shared_ptr;
    Enumeration(state_t, 0,
                init,
                read_header,
                read_data);
    session(stream_t _stream, uint32_t _id);
    inline void switching_state(state_t new_state)
    {
        state = new_state;
    }
    inline void clear_buffer()
    {
        bytes_transferred = 0;
        message.clear();
    }

public:
    stream_t stream;
    state_t  state;
    uint32_t id;
    protocol::message message;
    size_t   bytes_transferred;
    ::std::array<unsigned char, sizeof(message) * 2> buffer; //!ISSUE: vee2.0 RFC6455 헤더 버퍼 패치 후에 정확한 사이즈 사용 가능.
private:
    session() = delete;
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
    static void __stdcall _on_data_received(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const recieve_buffer_address, size_t recieve_buffer_size);
    static void __stdcall _header_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size);
    static void __stdcall _data_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size);
    static uint32_t __stdcall _generate_sid();
private:
    ::vee::net::net_server::shared_ptr _native_server;
    ::vee::net::net_server::shared_ptr _web_server;
};

} // !namespace comm

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_COMM_GATEWAY_H_

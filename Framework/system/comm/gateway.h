#ifndef _OPENNUI_SYSTEM_COMM_GATEWAY_H_
#define _OPENNUI_SYSTEM_COMM_GATEWAY_H_

#include <system/exceptions.h>
#include <opennui/protocol.h>
#include <vee/network/net.h>
#include <atomic>

namespace opennui {

namespace sys {

namespace comm {

class session
{
    DISALLOW_COPY_AND_ASSIGN(session);
public:
    using shared_ptr = ::std::shared_ptr<session>;
    using unqiue_ptr = ::std::unique_ptr<session>;
    using stream_t = ::std::shared_ptr<::vee::io::stream>;
    Enumeration(state_t, 0,
                init,
                read_header,
                read_data);
    session(stream_t _stream, uint32_t _id);
    session(session&& other);
    session& operator=(session&& rhs);
    ~session();
    inline void switching_state(state_t new_state)
    {
        state = new_state;
    }
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
    stream_t keep_alive_stream;
    stream_t cts_msg_stream;
    stream_t stc_msg_stream;
    state_t  state;
    uint32_t id;
    protocol::comm::client_type type;
    protocol::comm::message msgbuf_in;
    protocol::comm::message msgbuf_out;
    size_t   bytes_transferred_in;
    ::std::array<unsigned char, sizeof(protocol::comm::message) * 2> buffer_in; //!ISSUE: vee2.0 RFC6455 헤더 버퍼 패치 후에 정확한 사이즈 사용 가능.
    ::std::array<unsigned char, sizeof(protocol::comm::message) * 2> buffer_out; //!ISSUE: vee2.0 RFC6455 헤더 버퍼 패치 후에 정확한 사이즈 사용 가능.

private:
    session() = delete;
};

class gateway final
{
public:
    ~gateway();
    static void __stdcall open();
    static void __stdcall close();

private:
    using server_type = protocol::comm::client_type;
    using server_t = ::vee::net::net_server::shared_ptr;
    static gateway& _get_instance();
    gateway();
    static void __stdcall _on_client_connected(server_t server, server_type type,::vee::net::op_result&/*operation result*/, ::vee::net::net_stream::shared_ptr/*stream*/);
    static void __stdcall _on_data_received(session::shared_ptr session, ::vee::io::io_result& io_result, unsigned char* const recieve_buffer_address, size_t recieve_buffer_size);
    static void __stdcall _on_data_sent(session::shared_ptr session, ::vee::io::io_result& io_result);
    static void __stdcall _header_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size);
    static void __stdcall _data_processing(session::shared_ptr& session, ::vee::io::io_result& io_result, unsigned char* const buffer, size_t buffer_size);
    static uint32_t __stdcall _generate_sid();
    static void __stdcall _async_query_proc_launcher(session::shared_ptr& session) throw(...);
    class query_processing
    {
    public:
        static void __stdcall handshake(session::shared_ptr session, protocol::comm::message copied_msg) __noexcept;
    };
private:
    server_t _native_server;
    server_t _web_server;
    ::std::atomic<bool> _flag_open;
};

} // !namespace comm

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_COMM_GATEWAY_H_

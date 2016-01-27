#ifndef _OPENNUI_SYSTEM_FRDMGR_H_
#define _OPENNUI_SYSTEM_FRDMGR_H_

#include <system/virutual_device.h>
#include <vee/timer.h>
#include <vee/concurrency/syncronized_ringqueue.h>
#include <vee/enumeration.h>
#include <map>

namespace opennui {

namespace sys {

class frdmgr final
{
public:
    using timer_t = ::vee::async_timer;
    using timer_tick = timer_t::timer_tick;
    static const int msg_queue_capacity = 1024;
    ~frdmgr();
    struct frdmgr_msg
    {
        Enumeration(option, 100, 
                    create,
                    remove);
        option opt;
        virtual_device::shared_ptr vd;
        frdmgr_msg& operator=(frdmgr_msg&& other)
        {
            opt = other.opt;
            vd = ::std::move(other.vd);
            return *this;
        }
    };
    static void send_message(frdmgr_msg& msg);
    static void send_message(frdmgr_msg&& msg);
    static void send_message(frdmgr_msg::option opt, virtual_device::shared_ptr vd);
    static void stop();
    static void force_release_all_devices();
private:
    frdmgr();
    static void _on_timer(timer_tick tick);
    static frdmgr& _get_instance();
    ::vee::syncronized_ringqueue<frdmgr_msg> _msg_queue;
    ::vee::async_timer::shared_ptr _timer;
    ::std::map<virtual_device::key_t, virtual_device::shared_ptr> _devices;
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_FRDMGR_H_
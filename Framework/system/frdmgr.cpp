#include <system/frdmgr.h>
#pragma warning(disable:4127)
namespace opennui {

namespace sys {

frdmgr::~frdmgr()
{
    stop();
}

frdmgr::frdmgr():
_msg_queue(msg_queue_capacity)
{
    _timer = ::vee::create_timer();
    _timer->run(16, ::vee::make_delegate<void(timer_tick)>(_on_timer));
}

frdmgr& frdmgr::_get_instance()
{
    static frdmgr instance;
    return instance;
}

void frdmgr::send_message(frdmgr_msg& msg)
{
    frdmgr& inst = _get_instance();
    inst._msg_queue.enqueue(msg);
}

void frdmgr::send_message(frdmgr_msg&& msg)
{
    frdmgr& inst = _get_instance();
    inst._msg_queue.enqueue(::std::move(msg));
}

void frdmgr::send_message(frdmgr_msg::option opt, virtual_device::shared_ptr vd)
{
    frdmgr_msg msg;
    msg.opt = opt;
    msg.vd = ::std::move(vd);
    send_message(::std::move(msg));
}

void frdmgr::_on_timer(timer_tick tick)
{
    //logger::system_log("tick: %d", tick);
    frdmgr& inst = _get_instance();
    static frdmgr_msg msg;
    while (inst._msg_queue.dequeue(msg))
    {
        switch (msg.opt.enum_form())
        {
        case frdmgr_msg::option::create:
        {
            logger::system_log("Forward Manager consumes message\n\toption:\t%s\n\tname:\t%s\n\tvdkey:\t%d", msg.opt.to_string(), msg.vd->name(), msg.vd->key());
            inst._devices.insert(::std::make_pair(msg.vd->key(), msg.vd));
            break;
        }
        case frdmgr_msg::option::remove:
        {
            logger::system_log("Forward Manager consumes message\n\toption:\t%s\n\tname:\t%s\n\tvdkey:\t%d", msg.opt.to_string(), msg.vd->name(), msg.vd->key());
            inst._devices.erase(msg.vd->key());
            break;
        }
        default:
        {
            logger::system_error_log("Invalid option detected in the %s\n\tdetail: option: %d", __FUNCTION__, msg.opt.to_int32());
            break;
        }
        }
    }

    for (auto& it : inst._devices)
    {
        auto module_ptr = it.second->module_addr();
        {
            shared_buffer::shared_ptr buffer = it.second->color_frame_buffer();
            auto writable = buffer->get_writable_data_address(buffer->extension_next_write_idx, tick);
            bool result = module_ptr->acquire_color_frame(writable.first);
            if (result)
                memmove(buffer->get_specific_timestamp_address(writable.second), &tick, sizeof(uint32_t));
            buffer->unlock(writable.second);
            ++buffer->extension_next_write_idx %= buffer->block_count;
            //logger::system_log("write a color image from %s, idx: %d", it.second->name(), writable.second);
        }
        {
            shared_buffer::shared_ptr buffer = it.second->depth_frame_buffer();
            auto writable = buffer->get_writable_data_address(buffer->extension_next_write_idx, tick);
            bool result = module_ptr->acquire_depth_frame(writable.first);
            if (result)
                memmove(buffer->get_specific_timestamp_address(writable.second), &tick, sizeof(uint32_t));
            buffer->unlock(writable.second);
            ++buffer->extension_next_write_idx %= buffer->block_count;
            //logger::system_log("write a depth image from %s, idx: %d", it.second->name(), writable.second);
        }
    }
}

void frdmgr::stop()
{
    logger::system_log("Timer will be stop");
    frdmgr& inst = _get_instance();
    inst._timer->stop();
}

void frdmgr::force_release_all_devices()
{
    frdmgr& inst = _get_instance();
    stop();
    inst._devices.clear();
}

} // !namespace sys

} // !namespace opennui
#pragma warning(default:4127)
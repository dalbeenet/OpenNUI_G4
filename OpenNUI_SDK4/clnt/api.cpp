#include <opennui/clnt/api.h>
#include <opennui/clnt/logger.h>

namespace opennui {

namespace clnt {

namespace comm {

native_session::shared_ptr connect_to_local_framework() throw(...);

}

namespace api {

comm::native_session::shared_ptr __stdcall connect_to_framework() throw(...)
{
    return comm::connect_to_local_framework();
}

shared_buffer::shared_ptr __stdcall temp_get_color_buffer()
{
    return ::std::make_shared<shared_buffer>("Kinect2-color-buffer",
                                             1920 * 1080 * 4,
                                             15, 
                                             0);
}

shared_buffer::shared_ptr __stdcall temp_get_depth_buffer()
{
    return ::std::make_shared<shared_buffer>("Kinect2-depth-buffer",
                                             512 * 424 * 2,
                                             15,
                                             0);
}

void __stdcall temp_acquire_kinect2_color_frame(shared_buffer::shared_ptr sbuf, unsigned char* dst)
{
    static uint32_t ts = 0;
    auto readable = sbuf->get_readable_data_address(sbuf->extension_next_read_idx, ts);
    ts = *sbuf->get_specific_timestamp_address(readable.second);
    sbuf->unlock(readable.second);
    ++sbuf->extension_next_read_idx %= sbuf->block_count;
    memmove(dst, readable.first, 1920 * 1080 * 4);
    logger::system_log("read a color image from %s, idx: %d", sbuf->get_name(), readable.second);
}

void __stdcall temp_acquire_kinect2_depth_frame(shared_buffer::shared_ptr sbuf, unsigned char* dst)
{
    static uint32_t ts = 0;
    auto readable = sbuf->get_readable_data_address(sbuf->extension_next_read_idx, ts);
    ts = *sbuf->get_specific_timestamp_address(readable.second);
    sbuf->unlock(readable.second);
    ++sbuf->extension_next_read_idx %= sbuf->block_count;
    memmove(dst, readable.first, 512 * 424 * 2);
    logger::system_log("read a depth image from %s, idx: %d", sbuf->get_name(), readable.second);
}

}

}

}
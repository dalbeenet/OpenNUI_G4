#ifndef _OPENNUI_SDK4_CLNT_API_H_
#define _OPENNUI_SDK4_CLNT_API_H_

#include <opennui/clnt/comm/session.h>
#include <opennui/service/shared_buffer.h>

namespace opennui {

namespace clnt {

namespace api {

comm::native_session::shared_ptr __stdcall connect_to_framework();

shared_buffer::shared_ptr __stdcall temp_get_color_buffer();

shared_buffer::shared_ptr __stdcall temp_get_depth_buffer();

void __stdcall temp_acquire_kinect2_color_frame(shared_buffer::shared_ptr sbuf, unsigned char* dst);

void __stdcall temp_acquire_kinect2_depth_frame(shared_buffer::shared_ptr sbuf, unsigned char* dst);

} // !namespace api

} // !namespace clnt

} // !namespace opennui

#endif // !_OPENNUI_SDK4_CLNT_API_H_

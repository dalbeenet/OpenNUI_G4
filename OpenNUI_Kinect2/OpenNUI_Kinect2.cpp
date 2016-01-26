#include <opennui/opennui.h>
#include <Windows.h>

namespace opennui {

class kinect2: public opennui_device
{
public:
    kinect2()
    {
        //puts("Hello kinect2!");
    }
    DEFINE_OPENNUI_DEVICE(kinect2);
};

kinect2::~kinect2()
{
    //puts("Goodbye kinect2!");
}

bool kinect2::open()
{
    return false;
}

bool kinect2::suspend()
{
    return false;
}

bool kinect2::release()
{
    return false;
}

device_state kinect2::state() const
{
    return device_state::error;
}

void kinect2::name(::std::string& out) const
{
    out = "Kinect2";
}

void kinect2::vendor(::std::string& out) const
{
    out = "Microsoft";
}

void kinect2::uuid(::std::string& out) const
{
    out = "NULL";
}

void kinect2::rivision(::std::string& out) const
{
    out = "NULL";
}

int32_t kinect2::datatype_provided() const
{
    return 0;
}

void kinect2::get_color_frame_format(::opennui::nuidata::image::image_format& out) const
{

}

void kinect2::get_depth_frame_format(::opennui::nuidata::image::image_format& out) const
{

}

void kinect2::get_body_tracking_info(::opennui::nuidata::tracking::body::tracking_info) const
{

}

void kinect2::get_hand_tracking_info(::opennui::nuidata::tracking::hand::tracking_info) const
{

}

bool kinect2::acquire_color_frame(unsigned char* const out)
{
    return false;
}

bool kinect2::acquire_depth_frame(unsigned char* const out)
{
    return false;
}

bool kinect2::acquire_body_frame(unsigned char* const out)
{
    return false;
}

bool kinect2::acquire_hand_frame(unsigned char* const out)
{
    return false;
}

} // !namespace opennui

::opennui::kinect2 instance;

__declspec(dllexport) opennui::opennui_device* __stdcall opennui_main()
{
    return &instance;
}

BOOL WINAPI DllMain(HINSTANCE module_handle, DWORD reason_for_call, LPVOID reserved)
{
    switch (reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //puts("DLL_PROCESS_ATTACH");
        break;
    case DLL_PROCESS_DETACH:
        //puts("DLL_PROCESS_DETACH");
        break;
    case DLL_THREAD_ATTACH:
        //puts("DLL_THREAD_ATTACH");
        break;
    case DLL_THREAD_DETACH:
        //puts("DLL_THREAD_DETACH");
        break;
    default:
        //puts("INVALID REASON");
        break;
    }
    return TRUE;
}
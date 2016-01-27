#include <opennui/opennui.h>
#include <opennui/clnt/logger.h>
#include <Windows.h>
#include <Kinect.h>
#include <atomic>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

namespace opennui {

class kinect2: public opennui_device
{
public:
    kinect2()
    {
        //puts("Hello kinect2!");
    }
    DEFINE_OPENNUI_DEVICE(kinect2);
    IColorFrameReader*  pColorReader;
    IFrameDescription*  pColorDescription;
    IColorFrameSource*  pColorSource;
    IDepthFrameReader*  pDepthReader;
    IFrameDescription*  pDepthDescription;
    IDepthFrameSource*  pDepthSource;
    IBodyFrameReader*   pBodyReader;
    IBodyFrameSource*   pBodySource;
    IKinectSensor*      pSensor;
    ::std::atomic<device_state> _state = device_state::unloaded;
};

kinect2::~kinect2()
{
    //puts("Goodbye kinect2!");
}

bool kinect2::open()
{
    if (_state != device_state::unloaded)
    {
        return true;
    }

    // Sensor
    HRESULT hResult = S_OK;
    hResult = GetDefaultKinectSensor(&pSensor);
    if (FAILED(hResult))
    {
        clnt::logger::system_log("Error : GetDefaultKinectSensor");
        return false;
    }

    hResult = pSensor->Open();
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IKinectSensor::Open()");
        return false;
    }

    // Color
    hResult = pSensor->get_ColorFrameSource(&pColorSource);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IKinectSensor::get_ColorFrameSource()");
        return false;
    }
    hResult = pColorSource->OpenReader(&pColorReader);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IColorFrameSource::OpenReader()");
        return false;
    }

    hResult = pColorSource->get_FrameDescription(&pColorDescription);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IColorFrameSource::get_FrameDescription()");
        return false;
    }
    SafeRelease(pColorSource);

    // Depth
    hResult = pSensor->get_DepthFrameSource(&pDepthSource);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IKinectSensor::get_DepthFrameSource()");
        return false;
    }
    hResult = pDepthSource->OpenReader(&pDepthReader);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IDepthFrameSource::OpenReader()");
        return false;
    }
    SafeRelease(pDepthSource);
    // Body
    hResult = pSensor->get_BodyFrameSource(&pBodySource);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IKinectSensor::get_BodyFrameSource()");
        return false;
    }
    hResult = pBodySource->OpenReader(&pBodyReader);
    if (FAILED(hResult))
    {
        clnt::logger::system_log( "Error : IBodyFrameSource::OpenReader()");
        return false;
    }
    SafeRelease(pBodySource);
    _state = device_state::opened;

    return true;
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
    return _state;
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
    out.width = 1920;
    out.height = 1080;
    out.channel = 4;
    out.bytes_per_pixel = 4;
}

void kinect2::get_depth_frame_format(::opennui::nuidata::image::image_format& out) const
{
    out.width = 512;
    out.height = 424;
    out.channel = 1;
    out.bytes_per_pixel = 2;
}

void kinect2::get_body_tracking_info(::opennui::nuidata::tracking::body::tracking_info& out) const
{
    out.maximum_tracking_bodies = BODY_COUNT;
    out.number_of_joints = 100;
}

void kinect2::get_hand_tracking_info(::opennui::nuidata::tracking::hand::tracking_info& out) const
{

}

bool kinect2::acquire_color_frame(unsigned char* const out)
{
    bool result = false;
    IColorFrame* pColorFrame = nullptr;
    static unsigned int bufferSize = 1920 * 1080 * 4 * sizeof(unsigned char);

    HRESULT hResult = S_OK;
    hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, (BYTE*)out, ColorImageFormat::ColorImageFormat_Rgba);
        if (SUCCEEDED(hResult))
            result = true;
    }
    SafeRelease(pColorFrame);

#ifdef ENVIRONMENT64
    BYTE* dest = (BYTE*)out;
    for (int i = 0; i < 1920 * 1080; ++i)
    {
        BYTE r = *(dest + (i * 4));
        BYTE b = *(dest + (i * 4) + 2);
        *(dest + (i * 4)) = b;
        *(dest + (i * 4) + 2) = r;
    }
#else
    void* source = (BYTE*)out;
    __asm
    {
        mov ecx, 1920 * 1080    // Set Up A Counter (Dimensions Of Memory Block)
            mov ebx, source     // Points ebx To Our Data
label :
        mov al, [ebx + 0]
            mov ah, [ebx + 2]
            mov[ebx + 2], al
            mov[ebx + 0], ah
            add ebx, 4
            dec ecx             // Decreases Our Loop Counter
            jnz label           // If Not Zero Jump Back To Label
    }
#endif
    return result;
}

bool kinect2::acquire_depth_frame(unsigned char* const out)
{
    bool result = false;
    IDepthFrame* pDepthFrame = NULL;
    static unsigned int bufferSize = 512 * 424;
    HRESULT hResult = S_OK;

    hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pDepthFrame->CopyFrameDataToArray(bufferSize, (UINT16*)out);
        if (SUCCEEDED(hResult))
            result = true;
    }
    SafeRelease(pDepthFrame);

    return result;
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
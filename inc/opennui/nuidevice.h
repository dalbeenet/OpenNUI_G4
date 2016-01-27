#ifndef _OPENNUI_SDK4_NUIDEVICE_H_
#define _OPENNUI_SDK4_NUIDEVICE_H_

#include <opennui/nuidata.h>
#include <string>

namespace opennui {

enum class device_state: int32_t
{
    unloaded = 0,
    unknown,
    opened,
    suspended,
    error,
};

enum class nuidata_flag: int32_t
{
    depth = 0x01,
    color = depth << 1,
    body = color << 1,
    hand = body << 1,
    audio = hand << 1,
    etc = audio << 1,
};

class opennui_device abstract
{
public:
    virtual ~opennui_device() = default;
    virtual bool open() = 0;
    virtual bool suspend() = 0;
    virtual bool release() = 0;
    virtual ::opennui::device_state state() const = 0;
    virtual void name(::std::string& out) const = 0;
    virtual void vendor(::std::string& out) const = 0;
    virtual void uuid(::std::string& out) const = 0;
    virtual void rivision(::std::string& out) const = 0;
    virtual int32_t datatype_provided() const = 0;
    virtual void get_color_frame_format(::opennui::nuidata::image::image_format& out) const = 0;
    virtual void get_depth_frame_format(::opennui::nuidata::image::image_format& out) const = 0;
    virtual void get_body_tracking_info(::opennui::nuidata::tracking::body::tracking_info&) const = 0;
    virtual void get_hand_tracking_info(::opennui::nuidata::tracking::hand::tracking_info&) const = 0;
    //virtual void get_face_tracking_info() const = 0;
    virtual bool acquire_color_frame(unsigned char* const out) = 0;
    virtual bool acquire_depth_frame(unsigned char* const out) = 0;
    virtual bool acquire_body_frame(unsigned char* const out) = 0;
    virtual bool acquire_hand_frame(unsigned char* const out) = 0;
};

class opennui_plugin abstract
{
    virtual ~opennui_plugin() = default;
    virtual bool open() = 0;
    virtual bool suspend() = 0;
    virtual bool release() = 0;
    virtual ::opennui::device_state state() const = 0;
    virtual void name(::std::string& out) const = 0;
    virtual void vendor(::std::string& out) const = 0;
    virtual void uuid(::std::string& out) const = 0;
    virtual void rivision(::std::string& out) const = 0;
};

#define DEFINE_OPENNUI_DEVICE(class_name) public:\
    virtual ~##class_name();\
    virtual bool open() override;\
    virtual bool suspend() override;\
    virtual bool release() override;\
    virtual ::opennui::device_state state() const override;\
    virtual void name(::std::string& out) const override;\
    virtual void vendor(::std::string& out) const override;\
    virtual void uuid(::std::string& out) const override;\
    virtual void rivision(::std::string& out) const override;\
    virtual int32_t datatype_provided() const override;\
    virtual void get_color_frame_format(::opennui::nuidata::image::image_format& out) const override;\
    virtual void get_depth_frame_format(::opennui::nuidata::image::image_format& out) const override;\
    virtual void get_body_tracking_info(::opennui::nuidata::tracking::body::tracking_info&) const override;\
    virtual void get_hand_tracking_info(::opennui::nuidata::tracking::hand::tracking_info&) const override;\
    virtual bool acquire_color_frame(unsigned char* const out) override;\
    virtual bool acquire_depth_frame(unsigned char* const out) override;\
    virtual bool acquire_body_frame(unsigned char* const out) override;\
    virtual bool acquire_hand_frame(unsigned char* const out) override;

#undef OPENNUI_INTERFACE

} // !namespace opennui

#endif // !_OPENNUI_SDK4_NUIDEVICE_H_

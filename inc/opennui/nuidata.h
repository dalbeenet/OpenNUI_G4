#ifndef _OPENNUI_NUIDATA_H_
#define _OPENNUI_NUIDATA_H_

#include <cinttypes>
#include <array>
#include <memory>

namespace opennui {

using opennui_buffer = ::std::shared_ptr<unsigned char>;
using opennui_readonly_buffer = ::std::shared_ptr<const unsigned char>;

namespace nuidata {

enum class datatype: int32_t
{
    image,
    body_tracking,
    hand_tracking,
    audio,
    binary,
    text,
};

struct vector3d
{
    double x;
    double y;
    double z;
};

struct vector4f
{
    float x;
    float y;
    float z;
    float w;
};

enum class tracking_state: int32_t
{
    unknown = 0,
    not_tracked,
    inferred,
    tracked,
};

class joint
{
public:
    tracking_state state = tracking_state::unknown;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

namespace image {

struct header_field
{
public:
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channel = 0;
    uint32_t bytes_per_pixel = 0;
    inline unsigned int buffer_pitch() const
    {
        return width * height;
    }
    inline unsigned int size() const
    {
        return width * height * bytes_per_pixel;
    }
    inline unsigned int bit_depth() const
    {
        return (bytes_per_pixel * 8) / channel;
    }
};

struct data_frame
{
    header_field header_field;
    opennui_readonly_buffer data;
};

} // !namespace image

namespace tracking {

namespace body {

enum class joint_type: int32_t
{
    null = 0,
    spine_base,
    spine_mid,
    neck,
    head,
    shoulder_left,
    elbow_left,
    wrist_left,
    hand_left,
    shoulder_right,
    elbow_right,
    wrist_right,
    hand_right,
    hip_left,
    knee_left,
    ankle_left,
    foot_left,
    hip_right,
    knee_right,
    ankle_right,
    foot_right,
    spine_shoulder,
    handtip_left,
    thumb_left,
    handtip_right,
    thumb_right,
    number_of_joints
};

struct body_joint: public joint
{
    joint_type type = joint_type::null;
};

struct body_unit
{
    uint32_t id = 0;
    ::std::array<body_joint, (const int32_t)joint_type::number_of_joints> joints;
};

struct header_field
{
    uint32_t maximum_tracking_bodies = 0;
    uint32_t current_tracking_bodies = 0;
    uint32_t number_of_joints = 0;
};

struct data_frame
{
    header_field header;
    opennui_readonly_buffer raw_data;
    inline const body_unit& get_body_unit(uint32_t index)
    {
        uint32_t offset = sizeof(body_unit) * index;
        return *reinterpret_cast<const body_unit*>(const_cast<unsigned char*>(raw_data.get()) + offset);
    }
};

} // !namespace body

namespace hand {

enum class joint_type
{
    thumb_fingertip,
    thumb_jointC,
    thumb_jointB,
    thumb_jointA,
    index_fingertip,
    index_jointC,
    index_jointB,
    index_jointA,
    middle_fingertip,
    middle_jointC,
    middle_jointB,
    middle_jointA,
    ring_fingertip,
    ring_jointC,
    ring_jointB,
    ring_jointA,
    pinky_fingertip,
    pinky_jointC,
    pinky_jointB,
    pinky_jointA,
    palm,
    wrist,
    number_of_joints
};

enum class hand_state: int32_t
{
    unknown = 0,
    not_tracked,
    open,
    closed,
    point,
};

class hand_unit
{
    bool is_left;
    hand_state state;    
};

class header_field
{
public:
    uint32_t maximum_tracking_joints;
    uint32_t current_tracking_joints;
    uint32_t number_of_joints;
};

} // !namespace hand

namespace face {

} // !namespace face

} // !namespace tracking

using nuibody = tracking::body::body_unit;
using nuihand = tracking::hand::hand_unit;

} // !namespae nuidata

} // !namespace opennui

#endif // !_OPENNUI_NUIDATA_H_

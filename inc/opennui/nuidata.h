#ifndef _OPENNUI_SDK4_NUIDATA_H_
#define _OPENNUI_SDK4_NUIDATA_H_

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

struct image_format
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
    using header_field = image_format;
    header_field header;
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

struct nuibody
{
    bool     is_valid = false;
    uint32_t id = 0;
    ::std::array<body_joint, (const int32_t)joint_type::number_of_joints> joints;
};

struct tracking_info
{
    uint32_t maximum_tracking_bodies = 0;
    uint32_t current_tracking_bodies = 0;
    uint32_t number_of_joints = 0;
};

struct data_frame
{
    using header_field = tracking_info;
    header_field header;
    opennui_readonly_buffer raw_data;
    inline const nuibody& get_body_unit(uint32_t id)
    {
        uint32_t offset = sizeof(nuibody) * id;
        return *reinterpret_cast<const nuibody*>(const_cast<unsigned char*>(raw_data.get()) + offset);
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

enum class hand_type: int32_t
{
    unknown = 0,
    left,
    right,
};

class nuihand
{
    bool      is_valid = false;
    uint32_t  id = 0;
    hand_type is_left = hand_type::unknown;
    hand_state state = hand_state::unknown; 
};

class tracking_info
{
public:
    uint32_t maximum_tracking_joints;
    uint32_t current_tracking_joints;
    uint32_t number_of_joints;
};

struct data_frame
{
    using header_field = tracking_info;
    header_field header;
    opennui_readonly_buffer raw_data;
    inline const nuihand& get_left_hand(uint32_t id)
    {
        uint32_t offset = sizeof(nuihand) * id;
        return *reinterpret_cast<const nuihand*>(const_cast<unsigned char*>(raw_data.get()) + offset);
    }
    inline const nuihand& get_right_hand(uint32_t id)
    {
        uint32_t offset = sizeof(nuihand) * (id + 1);
        return *reinterpret_cast<const nuihand*>(const_cast<unsigned char*>(raw_data.get()) + offset);
    }
};

} // !namespace hand

namespace face {

} // !namespace face

} // !namespace tracking

using nuibody = tracking::body::nuibody;
using nuihand = tracking::hand::nuihand;

} // !namespae nuidata

} // !namespace opennui

#endif // !_OPENNUI_SDK4_NUIDATA_H_

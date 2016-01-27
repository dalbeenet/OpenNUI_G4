#ifndef _OPENNUI_SYSTEM_SHARED_BUFFER_H_
#define _OPENNUI_SYSTEM_SHARED_BUFFER_H_

#include <system/sysbase.h>
#include <vee/interprocess/shared_memory.h>
#include <memory>

namespace opennui {

namespace sys {

class shared_buffer
{
    DISALLOW_COPY_AND_ASSIGN(shared_buffer);
    DISALLOW_MOVE_AND_ASSIGN(shared_buffer);
public:
    using shared_ptr = ::std::shared_ptr<shared_buffer>;
    using unique_ptr = ::std::unique_ptr<shared_buffer>;
    shared_buffer(const char* shm_name, size_t block_size, size_t block_count);
    inline unsigned char* get_address() const
    {
        return reinterpret_cast<unsigned char*>(_shm->get_address());
    }
    ~shared_buffer() = default;
private:
    ::vee::interprocess::shared_memory::shared_ptr _shm;
};

} // !namespace sys

} // !namespace opennui

#endif // !_OPENNUI_SYSTEM_SHARED_BUFFER_H_

#ifndef _OPENNUI_SDK4_SERVICE_SHARED_BUFFER_H_
#define _OPENNUI_SDK4_SERVICE_SHARED_BUFFER_H_

#include <vee/interprocess/shared_memory.h>
#include <memory>

namespace opennui {

class shared_buffer
{
    DISALLOW_COPY_AND_ASSIGN(shared_buffer);
    DISALLOW_MOVE_AND_ASSIGN(shared_buffer);
public:
    static const int metadata_block_size = 1024;
    using lock_t = uint32_t;
    using timestamp_t = uint32_t;
    using shared_ptr = ::std::shared_ptr<shared_buffer>;
    using unique_ptr = ::std::unique_ptr<shared_buffer>;
    shared_buffer(const char* shm_name, size_t block_size_, size_t block_count_, uint32_t key_);
    ~shared_buffer() = default;
    inline const char* get_name() const
    {
        return _shm->get_name();
    }
    inline unsigned char* get_address() const
    {
        return reinterpret_cast<unsigned char*>(_shm->get_address());
    }
    inline unsigned char* get_specific_data_block_address(int index)
    {
        return get_address() + data_offset + (index * block_size);
    }
    inline lock_t* get_specific_lock_address(int index)
    {
        return reinterpret_cast<lock_t*>(get_address() + lock_offset + (index * sizeof(lock_t)));
    }
    inline timestamp_t* get_specific_timestamp_address(int index)
    {
        return reinterpret_cast<timestamp_t*>(get_address() + timestamp_offset + (index * sizeof(timestamp_t)));
    }
    inline bool index_validate(int index)
    {
        if (index >= 0 && index < (int)block_count)
            return true;
        return false;
    }
    bool lock(int index);
    bool unlock(int index);
    timestamp_t get_timestamp(int index);
    ::std::pair<unsigned char*, int> get_writable_data_address(int last_index, timestamp_t ts);
    ::std::pair<unsigned char*, int> get_readable_data_address(int last_index, timestamp_t ts);
    const size_t block_size;
    const size_t block_count;
    const size_t shm_size;
    const size_t data_offset;
    const size_t lock_offset;
    const size_t timestamp_offset;
    const uint32_t key;
    const lock_t locked;
    const lock_t unlocked;
    int extension_next_write_idx = 0;
    int extension_next_read_idx = 0;
private:
    ::vee::interprocess::shared_memory::shared_ptr _shm;
};

} // !namespace opennui

#endif // !_OPENNUI_SDK4_SERVICE_SHARED_BUFFER_H_

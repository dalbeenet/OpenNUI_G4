#include <opennui/service/shared_buffer.h>
#include <string>
#include <Windows.h>
#pragma warning(disable:4127)
namespace opennui {

shared_buffer::shared_buffer(const char* shm_name, size_t block_size_, size_t block_count_, uint32_t key_):
block_size(block_size_),
block_count(block_count_),
shm_size(metadata_block_size + (sizeof(lock_t) + sizeof(timestamp_t) + block_size) * block_count),
data_offset(metadata_block_size + (sizeof(lock_t) + sizeof(timestamp_t)) * block_count),
lock_offset(metadata_block_size),
timestamp_offset(metadata_block_size + sizeof(lock_t) * block_count),
key(key_),
locked(1),
unlocked(0)
{
    _shm = ::vee::interprocess::create_shared_memory(shm_name, shm_size);
    memset(_shm->get_address(), 0, shm_size);
    memmove(_shm->get_address(), _shm->get_name(), strlen(_shm->get_name()));
}

bool shared_buffer::lock(int index)
{
    if (!index_validate(index))
        return false;
    lock_t* dstptr = get_specific_lock_address(index);
    if (InterlockedCompareExchange(dstptr, locked, unlocked) == unlocked)
        return true;
    else
        return false;
}

bool shared_buffer::unlock(int index)
{
    if (!index_validate(index))
        return false;
    lock_t* dstptr = get_specific_lock_address(index);
    if (InterlockedCompareExchange(dstptr, unlocked, locked) == locked)
        return true;
    else
        return false;
}

::std::pair<unsigned char*, int> shared_buffer::get_writable_data_address(int last_index, timestamp_t ts)
{
    int ci = last_index;
    while (true)
    {
        if (lock(ci))
        {
            timestamp_t cits = get_timestamp(ci);
            if (cits < ts)
                break;
            unlock(ci);
        }
        ++ci %= block_count;
    }
    return ::std::make_pair(get_specific_data_block_address(ci), ci);
}

::std::pair<unsigned char*, int> shared_buffer::get_readable_data_address(int last_index, timestamp_t ts)
{
    int ci = last_index;
    while (true)
    {
        if (lock(ci))
        {
            timestamp_t cits = get_timestamp(ci);
            if (cits > ts)
                break;
            unlock(ci);
        }
        ++ci %= block_count;
    }
    return ::std::make_pair(get_specific_data_block_address(ci), ci);
}

shared_buffer::timestamp_t shared_buffer::get_timestamp(int index)
{
    return *(get_specific_timestamp_address(index));
}

} // !namespace opennui
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <mutex>
#include <cactus_rt/mutex.h>

// Thread-safe wrapper for a struct or class
// This is defined in the header file because it is a template
template <typename T>
class SharedMemory
{
private:
    T data_;                         // Instance of the struct or class
    mutable cactus_rt::mutex mutex_; // Mutex to synchronize access

public:
    // Constructor to initialize the data
    explicit SharedMemory(T initial_data = T()) : data_(std::move(initial_data)) {}

    // Thread-safe read: returns a copy of the data
    T Read() const
    {
        std::scoped_lock lock(mutex_);
        return data_; // Return a copy to ensure thread safety
    }

    // Thread-safe write: allows modifying the data
    void Write(const T &new_data)
    {
        std::scoped_lock lock(mutex_);
        data_ = new_data;
    }
};

#endif // SHARED_MEMORY_H
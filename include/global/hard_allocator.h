//
// Created by lby on 2018/3/11.
//

#ifndef TENSORSTACK_GLOBAL_ALLOCATOR_H
#define TENSORSTACK_GLOBAL_ALLOCATOR_H

#include "core/device.h"
#include "utils/except.h"

#include <functional>
#include <sstream>
#include <cmath>

#include <iomanip>

namespace ts {
    class OutOfMemoryException : public Exception {
    public:
        explicit OutOfMemoryException(const MemoryDevice &device, size_t failed_size)
                : Exception(OutOfMemoryMessage(device, failed_size)), m_device(device), m_failed_size(failed_size) {
        }

        static std::string OutOfMemoryMessage(const MemoryDevice &device, size_t failed_size) {
            std::ostringstream oss;
            oss << "No enough memory on " << device
                << ", " << failed_size << "B needed.";
            return oss.str();
        }

        const Device &device() const {
            return m_device;
        }

        size_t failed_size() const {
            return m_failed_size;
        }

    private:
        MemoryDevice m_device;
        size_t m_failed_size;
    };

    class HardAllocator {
    public:
        /**
         * Memory allocator type, allocate memory from specific device
         * @see HardAllocatorDeclaration
         */
        using function = std::function<void *(int, size_t, void *)>;

        /**
         * Example of HardAllocator
         * @param id the allocating device id
         * @param size the new size of memory
         * @param mem the older memory
         * @return a pointer to new memory
         * @note if size == 0: free(mem),
         *        else if mem == nullptr: return malloc(size)
         *        else: return realloc(mem, size)
         */
        void *HardAllocatorFunction(int id, size_t size, void *mem);

        /**
         * Query memory allocator
         * @param device_type querying device
         * @return allocator
         * @note supporting called by threads without calling @sa RegisterDeviceAllocator or @sa RegisterAllocator
         * @note the query device should be memory device, you may call @sa QueryMemoryDevice to get memory device by compute device
         */
        static function Query(const DeviceType &device_type) TS_NOEXCEPT;

        /**
         * Register allocator for specific device type
         * @param device_type specific @sa DeviceType
         * @param allocator setting allocator
         * @note only can be called before running
         */
        static void Register(const DeviceType &device_type, const function &allocator) TS_NOEXCEPT;

        /**
         * No details for this API, so DO NOT call it
         */
        static void Clear();
    };
}

#endif //TENSORSTACK_GLOBAL_ALLOCATOR_H
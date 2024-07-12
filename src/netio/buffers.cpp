/**
 * @file buffers.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdexcept>
#include "netio/buffers.hpp"

namespace ToyServer::NetIO
{
    char& FixedBuffer::getAt(std::size_t pos)
    {
        if (pos < 0 || pos >= getCapacity())
            throw std::invalid_argument {"FixedBuffer: Invalid index OOB."};

        return getBasePtr()[pos];
    }

    void FixedBuffer::clearData() noexcept
    {
        octet_ptr_t base_ptr = getBasePtr();
        std::fill(base_ptr, base_ptr + capacity, '\0');
    }
}

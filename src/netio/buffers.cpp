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
        length = 0;
    }

    [[nodiscard]] bool FixedBuffer::loadChars(const std::string& content)
    {
        std::size_t content_len = content.length();

        if (content_len == 0)
            return true;

        if (content_len >= capacity)
            return false;

        octet_ptr_t dst_begin = getBasePtr();
        const char* content_begin = content.c_str();

        clearData();
        std::copy(content_begin, content_begin + content.length(), dst_begin);
        length += content_len;

        return true;
    }

    bool FixedBuffer::loadChars(const char* octet_ptr, std::size_t len)
    {
        if (len == 0)
            return true;

        if (len >= capacity)
            return false;

        octet_ptr_t dst_begin = getBasePtr();
        clearData();
        std::copy(octet_ptr, octet_ptr + len, dst_begin);
        length += len;

        return true;
    }
}

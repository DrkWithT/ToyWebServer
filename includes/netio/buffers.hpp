#ifndef BUFFERS_HPP
#define BUFFERS_HPP

#include <stdexcept>
#include <algorithm>
#include <memory>

namespace ToyServer::NetIO
{
    template <std::size_t N>
    class FixedBuffer
    {
    private:
        using octet_ptr_t = char*;

        std::unique_ptr<char> block;
        static constexpr std::size_t capacity = N + 1;

    public:
        constexpr FixedBuffer()
        : block {std::make_unique<char>(capacity)} {}

        FixedBuffer(const FixedBuffer& other) = delete;
        FixedBuffer& operator=(const FixedBuffer& other) = delete;

        constexpr std::size_t getCapacity() const noexcept { return capacity; }

        constexpr octet_ptr_t getBasePtr() const noexcept { return block.get(); }

        constexpr char& operator[](int pos) noexcept
        {
            return getBasePtr()[pos];
        }

        char& getAt(int pos)
        {
            if (pos < 0 || pos >= getCapacity())
                throw std::invalid_argument {"FixedBuffer::getAt: OOB access detected"};

            return getBasePtr()[pos];
        }

        constexpr void clearData() noexcept
        {
            constexpr octet_ptr_t base_ptr = getBasePtr();
            std::fill(base_ptr, base_ptr + N, '\0');
        }
    };
}

#endif
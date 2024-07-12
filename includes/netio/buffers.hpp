#ifndef BUFFERS_HPP
#define BUFFERS_HPP

#include <algorithm>
#include <memory>

namespace ToyServer::NetIO
{
    class FixedBuffer
    {
    private:
        using octet_ptr_t = char*;

        std::unique_ptr<char> block;
        std::size_t capacity;

    public:
        constexpr FixedBuffer(std::size_t capacity_)
        : block {std::make_unique<char>(capacity_)} {}

        FixedBuffer(const FixedBuffer& other) = delete;
        FixedBuffer& operator=(const FixedBuffer& other) = delete;

        constexpr std::size_t getCapacity() const noexcept { return capacity; }

        constexpr octet_ptr_t getBasePtr() const noexcept { return block.get(); }

        constexpr char& operator[](std::size_t pos) noexcept
        {
            return getBasePtr()[pos];
        }

        char& getAt(std::size_t pos);

        void clearData() noexcept;
    };
}

#endif
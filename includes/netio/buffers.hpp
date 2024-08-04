#ifndef BUFFERS_HPP
#define BUFFERS_HPP

#include <utility>
#include <string>
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

        constexpr void swapState(FixedBuffer&& other) noexcept
        {
            block = std::move(other.block);

            std::size_t temp_capacity = 0;
            std::swap(temp_capacity, other.capacity);
            capacity = temp_capacity;
        }
    public:
        constexpr FixedBuffer(std::size_t capacity_)
        : block {std::make_unique<char>(capacity_ + 1)} {}

        constexpr FixedBuffer(const FixedBuffer& other)
        {
            if (&other == this)
                return;

            block = std::make_unique<char>(other.capacity);

            std::copy(other.block.get(), other.block.get() + other.capacity, block.get());
            capacity = other.capacity;
        }

        constexpr FixedBuffer& operator=(const FixedBuffer& other)
        {
            if (&other == this)
                return *this;

            block = std::make_unique<char>(other.capacity);

            std::copy(other.block.get(), other.block.get() + other.capacity, block.get());
            capacity = other.capacity;

            return *this;
        }

        constexpr FixedBuffer(FixedBuffer&& other) noexcept
        {
            swapState(std::forward<FixedBuffer&&>(other));
        }

        constexpr FixedBuffer& operator=(FixedBuffer&& other) noexcept
        {
            swapState(std::forward<FixedBuffer&&>(other));
            return *this;
        }

        constexpr std::size_t getCapacity() const noexcept { return capacity; }

        constexpr octet_ptr_t getBasePtr() const noexcept { return block.get(); }

        constexpr char& operator[](std::size_t pos) noexcept
        {
            return getBasePtr()[pos];
        }

        char& getAt(std::size_t pos);

        void clearData() noexcept;

        [[nodiscard]] bool loadChars(const std::string& content);
        [[nodiscard]] bool loadChars(const char* octet_ptr, std::size_t len);
    };
}

#endif
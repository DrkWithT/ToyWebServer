#ifndef SYNQUEUE_HPP
#define SYNQUEUE_HPP

#include <type_traits>
#include <queue>
#include <mutex>

namespace ToyServer::Core
{
    template <typename ItemTp>
    class SyncedQueue
    {
    private:
        using naked_t = std::remove_reference_t<std::remove_cv_t<ItemTp>>;

        std::queue<naked_t> items; // contain tasks
        std::mutex mtx;            // mutex for synchronization of read/write
        int limit;

        [[nodiscard]] bool isEmpty() const
        {
            return items.empty();
        }

        [[nodiscard]] bool isFull() const
        {
            return items.size() >= limit;
        }

    public:
        explicit SyncedQueue(int limit_) noexcept
        : items {}, mtx {}, limit {limit_} {}

        [[nodiscard]] bool addItem(const naked_t& item)
        {
            std::lock_guard guard {mtx};
            bool size_ok = !isFull();

            if (size_ok)
                items.push(item);

            return size_ok;
        }

        [[nodiscard]] naked_t getItem()
        {
            naked_t temp {};

            {
                std::lock_guard guard {mtx};

                if (!isEmpty())
                {
                    temp = items.front();
                    items.pop();
                }
            }

            return temp;
        }
    };
}

#endif
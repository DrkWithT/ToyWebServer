#ifndef TASK_HPP
#define TASK_HPP

#include "netio/config.hpp"

namespace ToyServer::Core
{
    enum class TaskTag
    {
        tag_normal,       // usable socket task
        tag_placeholder,  // placeholder for invalid result from empty queue
        tag_halt_work     // worker "poison"
    };

    /**
     * @brief Task struct for an accepted client connection.
     * @note Uses `TaskTag` for a worker's to-do indicator.
     */
    struct ConnectionTask
    {
        NetIO::SocketConfig config; // socket data to consume
        TaskTag tag;                // action tag for a consumer

        constexpr ConnectionTask() noexcept
        : config {}, tag {TaskTag::tag_placeholder} {}

        constexpr ConnectionTask(NetIO::SocketConfig config_, TaskTag tag_) noexcept
        : config {config_}, tag {tag_} {}
    };
}

#endif
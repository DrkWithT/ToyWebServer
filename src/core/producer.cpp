/**
 * @file producer.cpp
 * @author DrkWithT
 * @brief Implements connection task-producer logic. The callable's methods are implemented here, including SIGINT handling support by an atomic flag.
 * @date 2024-08-10
 */

#include <thread>
#include "core/producer.hpp"

namespace ToyServer::Core
{
    using namespace std::chrono_literals;

    static constexpr int server_socket_wait = 5;

    ServerProducer::ServerProducer(NetIO::SocketConfig config, int consumer_count_)
    : entry_socket {config, server_socket_wait}, consumer_count {consumer_count_} {}

    void ServerProducer::submitPoisonTasks(SyncedQueue<ConnectionTask>& queue, std::condition_variable_any& wait_var)
    {
        queue.clearAll();

        for (int submit_count = 0; submit_count < consumer_count; submit_count++)
        {
            queue.addItem(ConnectionTask {NetIO::SocketConfig {}, TaskTag::tag_halt_work});
            wait_var.notify_one();
            std::this_thread::sleep_for(0.0625s);
        }
    }

    void ServerProducer::operator()(SyncedQueue<ConnectionTask>& queue, std::atomic_bool& signal_flag, std::condition_variable_any& wait_var)
    {
        while (true)
        {
            if (signal_flag.load())
            {
                submitPoisonTasks(queue, wait_var);
                break;
            }

            auto connection_config = entry_socket.acceptConnection();

            /// @note By the manpages, accept(...) returns FD -1 on an error. Gracefully handle this with a placeholder task that is skipped.
            TaskTag tag = (connection_config.socket_fd != -1)
                ? TaskTag::tag_normal
                : TaskTag::tag_placeholder;

            queue.addItem(ConnectionTask {connection_config, tag});
            wait_var.notify_one();
        }
    }
}

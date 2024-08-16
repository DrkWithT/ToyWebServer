#ifndef PRODUCER_HPP
#define PRODUCER_HPP

#include <atomic>
#include <condition_variable>
#include "netio/config.hpp"
#include "netio/sockets.hpp"
#include "core/task.hpp"
#include "core/synqueue.hpp"

namespace ToyServer::Core
{
    class ServerProducer
    {
    private:
        NetIO::ServerSocket entry_socket;
        int consumer_count;

        void submitPoisonTasks(SyncedQueue<ConnectionTask>& queue, std::condition_variable_any& wait_var);

    public:
        explicit ServerProducer(NetIO::SocketConfig config, int consumer_count_);

        void operator()(SyncedQueue<ConnectionTask>& queue, std::atomic_bool& signal_flag, std::condition_variable_any& wait_var);
    };
}

#endif
/**
 * @file main.cpp
 * @author DrkWithT
 * @brief Implements main driver code for server.
 * @version 0.0.1
 * @date 2024-08-15
 */
#include <iostream>
#include <atomic>             // for SIGINT flag
#include <condition_variable> // for notifying workers of work
#include <thread>

#include "netio/config.hpp"
#include "core/producer.hpp"
#include "core/worker.hpp"

class MyServer
{
private:
    static constexpr std::size_t worker_count = 2;
    static constexpr std::size_t queue_limit = 4;

    ToyServer::Core::ServerWorker worker_0;
    ToyServer::Core::ServerWorker worker_1;
    ToyServer::Core::SyncedQueue<ToyServer::Core::ConnectionTask> tasks;
    ToyServer::Core::ServerProducer producer;

public:
    MyServer(ToyServer::NetIO::SocketConfig netio_config)
    : worker_0 {}, worker_1 {}, tasks {queue_limit}, producer {netio_config, worker_count} {}

    void runService()
    {
        std::thread prod_thread {producer};
        std::thread w0_thread {worker_0};
        std::thread w1_thread {worker_1};

        std::cout << "Starting server...\n";

        w0_thread.join();
        w1_thread.join();
        prod_thread.join();
    }
};

int main()
{
    // setup configuration options for server socket
    ToyServer::NetIO::SocketHints hints {"8080", 4, 4000};
    ToyServer::NetIO::AddrInfo options {hints};

    // use 1st option for demo purposes
    auto my_option = options.getNextOption();

    // test run server
    MyServer server {my_option.value()};
    server.runService();
}
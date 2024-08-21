/**
 * @file main.cpp
 * @author DrkWithT
 * @brief Implements main driver code for server.
 * @version 0.0.1
 * @date 08-20-2024
 */
#include <iostream>
#include <utility>
#include <atomic>             // for SIGINT flag
#include <condition_variable> // for notifying workers of work
#include <mutex>
#include <thread>
#include <csignal>
#include <unistd.h>

#include "netio/config.hpp"
#include "core/producer.hpp"
#include "core/worker.hpp"

std::atomic_bool interrupt_flag {false};
std::condition_variable_any waiting_flag;
std::mutex waiting_flag_mtx;

void handleIntSignal(int signal_id)
{
    if (signal_id == SIGINT)
    {
        interrupt_flag.store(true);
        write(STDOUT_FILENO, "Sent SIGINT for halting, please wait.\n", 38);
    }
}

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

    void runService(std::atomic_bool& signal_flag, std::condition_variable_any& wait_var, std::mutex& wait_mtx)
    {
        std::thread prod_thread {producer, std::ref(signal_flag), std::ref(wait_var)};
        std::thread w0_thread {worker_0, std::ref(wait_mtx), std::ref(wait_var)};
        std::thread w1_thread {worker_1, std::ref(wait_mtx), std::ref(wait_var)};

        std::cout << "Starting server...\n";

        w0_thread.join();
        w1_thread.join();
        prod_thread.join();

        std::cout << "Finished.\n";
    }
};

int main()
{
    // setup configuration options for server socket host, then client socket timeout
    ToyServer::NetIO::SocketHints hints {"8080", 4, 8};
    ToyServer::NetIO::AddrInfo options {hints};

    std::signal(SIGINT, handleIntSignal);

    // use 1st option for demo purposes
    auto my_option = options.getNextOption();

    // test run server
    MyServer server {my_option.value()};
    server.runService(interrupt_flag, waiting_flag, waiting_flag_mtx);
}
#ifndef WORKER_HPP
#define WORKER_HPP

#include "netio/sockets.hpp"
#include "http1/helpers.hpp"
#include "http1/messages.hpp"
#include "http1/reader.hpp"
#include "http1/writer.hpp"
#include "uri/url.hpp"

#include "core/task.hpp"
#include "core/synqueue.hpp"

namespace ToyServer::Core
{
    enum class ServingState : uint8_t
    {
        idle,    // nothing (during init.)
        meet,    // accept client
        request, // read request from client
        process, // process request & prepare reply
        reply,   // reply to client
        repeat,  // repeat request-response cycle
        close,   // close current connection and resume
        end      // stop FSM loop, no helper needed
    };

    class ServerWorker
    {
    private:
        Http1::HttpReader reader;           // HTTP msg parser & reader
        Http1::HttpWriter writer;           // HTTP msg serializer & sender
        Http1::Request req;                 // reusable HTTP request
        Http1::Response res;                // reusable HTTP response

        NetIO::ClientSocket session_socket; // accepted socket
        ServingState state;                 // loop state
        bool persist;

        ServingState actionMeet(SyncedQueue<ConnectionTask>& queue_ref);
        ServingState actionRequest();
        ServingState actionProcess();
        ServingState actionReply();
        ServingState actionRepeat();
        ServingState actionClose();

    public:
        explicit ServerWorker() noexcept;

        [[nodiscard]] bool resetSocket(ConnectionTask task);

        /// @note Use with `std::thread`, but make sure to `thread.join()` it.
        void operator()(SyncedQueue<ConnectionTask>& queue_ref);
    };
}

#endif
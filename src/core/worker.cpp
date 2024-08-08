/**
 * @file worker.cpp
 * @author DrkWithT
 * @brief Implements consumer callable for request-response handling workers.
 * @date 2024-08-06
 */

#include <chrono>
#include <string>
#include <map>
#include <thread>
#include "core/worker.hpp"

namespace ToyServer::Core
{
    using namespace std::chrono_literals;

    /* Constants */

    /// @brief Demo `Content-Length` header value for sample content aka `"Hello World!"`
    static constexpr const char* dud_content = "Hello World!";
    static constexpr std::size_t dud_content_length = 12;

    /* ServerWorker private impl. */

    ServingState ServerWorker::actionMeet(SyncedQueue<ConnectionTask>& queue_ref)
    {
        ConnectionTask task = queue_ref.getItem();

        if (task.tag == TaskTag::tag_halt_work)
        {
            return ServingState::end;
        }

        if (!resetSocket(task))
        {
            std::this_thread::sleep_for(1s);
            return ServingState::repeat;
        }

        return ServingState::request;
    }

    ServingState ServerWorker::actionRequest()
    {
        ServingState next = ServingState::idle;

        try
        {
            req = reader.nextRequest();
            next = ServingState::process;
        }
        catch (const std::runtime_error& io_error)
        {
            next = ServingState::close;
        }

        return next;
    }

    ServingState ServerWorker::actionProcess()
    {
        Http1::Schema http_version = req.schema;
        Http1::Method http_verb = req.method;
        const std::string& request_path = req.route.path;
        bool keep_alive = req.headers.at("Connection:") == "Keep-Alive";

        std::map<std::string, std::string> headers {};
        NetIO::FixedBuffer foo {dud_content_length};
        Http1::Status res_status = Http1::Status::last;

        switch (http_verb)
        {
        case Http1::Method::h1_get:
            /// @note Just handle GET / HTTP/1.1 for testing.
            if (request_path == "/")
            {
                // make demo headers
                headers["Content-Length"] = std::to_string(dud_content_length);
                headers["Content-Type"] = "text/plain";
                headers["Server"] = "ToyHttpD/0.0.1";
                res_status = Http1::Status::stat_ok;

                // make demo content
                foo.loadChars(dud_content, dud_content_length);
            }
            else
            {
                // don't handle non-root URLs for now, just 404
                headers["Content-Length"] = "0";
                headers["Content-Type"] = "text/plain";
                headers["Server"] = "ToyHttpD/0.0.1";
                res_status = Http1::Status::stat_not_found;
            }
            break;
        default:
            /// @note make demo headers but no body needed for non-GET reqs. for now!
            headers["Content-Length"] = "0";
            headers["Content-Type"] = "text/plain";
            headers["Server"] = "ToyHttpD/0.0.1";
            res_status = Http1::Status::stat_not_implemented;
            break;
        }

        headers["Connection"] = (keep_alive)
            ? "Keep-Alive"
            : "No-Persist";

        /// @note re-fill long-lived response object... I'd rather not reallocate its bulk for every request-response cycle.
        res = {http_version, res_status, "", headers, foo};
        persist = keep_alive;

        return ServingState::reply;
    }

    ServingState ServerWorker::actionReply()
    {
        try
        {
            writer.writeReply(res);

            return (persist)
                ? ServingState::repeat
                : ServingState::close;
        }
        catch (const std::runtime_error& io_error)
        {
            return ServingState::close;
        }
    }

    ServingState ServerWorker::actionRepeat()
    {
        return ServingState::request;
    }

    ServingState ServerWorker::actionClose()
    {
        writer.resetState(nullptr);
        reader.resetState(nullptr);

        NetIO::ClientSocket x_closer {};

        /// @note use RAII of short lived x_closer to close the original fd of this connection!
        x_closer = std::move(session_socket);

        return ServingState::meet;
    }

    /* ServerWorker public impl. */

    ServerWorker::ServerWorker() noexcept
    : reader {}, writer {nullptr}, req {}, res {}, session_socket {}, state {ServingState::idle}, persist {false} {}

    bool ServerWorker::resetSocket(ConnectionTask task)
    {
        auto [socket_config, tag] = task;

        if (tag == TaskTag::tag_placeholder)
        {
            /// @note destroy invalid connection task by temporary socket disposing it
            NetIO::ClientSocket x_closer {socket_config};
            return false;
        }

        this->session_socket = NetIO::ClientSocket {socket_config};

        return true;
    }

    void ServerWorker::operator()(SyncedQueue<ConnectionTask>& queue_ref)
    {
        while (state != ServingState::end)
        {
            switch (state)
            {
            case ServingState::idle:
                state = ServingState::meet;
                break;
            case ServingState::meet:
                state = actionMeet(queue_ref);
                break;
            case ServingState::request:
                state = actionRequest();
                break;
            case ServingState::process:
                state = actionProcess();
                break;
            case ServingState::reply:
                state = actionReply();
                break;
            case ServingState::repeat:
                state = actionRepeat();
                break;
            case ServingState::close:
                state = actionClose();
                break;
            default:
                break;
            }
        }
    }
}
#ifndef WRITER_HPP
#define WRITER_HPP

#include "netio/buffers.hpp"
#include "netio/sockets.hpp"
#include "http1/helpers.hpp"
#include "http1/messages.hpp"
#include "http1/serializer.hpp"

namespace ToyServer::Http1
{
    using NetIO::FixedBuffer;
    using NetIO::ClientSocket;

    /**
     * @brief Helper to write an HTTP/1.x request to a web client.
     */
    class HttpWriter
    {
    private:
        FixedBuffer out_buf;  // reply output buffer
        ClientSocket* socket; // non-owning pointer to use a reader and writer's client socket

    public:
        explicit HttpWriter() noexcept;

        HttpWriter(const HttpWriter& other) = delete;
        HttpWriter& operator=(const HttpWriter& other) = delete;

        [[nodiscard]] bool writeReply(const Response& req);
    };
}

#endif
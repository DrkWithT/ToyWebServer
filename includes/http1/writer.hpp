#ifndef WRITER_HPP
#define WRITER_HPP

#include "netio/buffers.hpp"
#include "netio/sockets.hpp"
#include "http1/helpers.hpp"
#include "http1/messages.hpp"

namespace ToyServer::Http1
{
    using NetIO::FixedBuffer;
    using NetIO::ClientSocket;

    [[nodiscard]] std::string_view stringifySchema(Schema schema);

    [[nodiscard]] std::string_view stringifyStatus(Status status);

    /**
     * @brief Helper to write an HTTP/1.x request to a web client.
     * @note Throws std::runtime_error on socket I/O failures.
     */
    class HttpWriter
    {
    private:
        FixedBuffer out_buf;  // reply output buffer
        ClientSocket* socket; // non-owning pointer for socket shared by reader, writer
        std::size_t buf_count; // octet count within buffer after pre-load

        void dumpOutBuffer(std::size_t load_count);

        void loadChars(const std::string& content);

        void loadChars(const char* octets_ptr, std::size_t len);

        void writeLines(const Response& res);

        void writePayload(const Response& res);

    public:
        explicit HttpWriter(ClientSocket* socket_ptr) noexcept;

        HttpWriter(const HttpWriter& other) = delete;
        HttpWriter& operator=(const HttpWriter& other) = delete;

        void writeReply(const Response& res);
    };
}

#endif
/**
 * @file writer.cpp
 * @author DrkWithT
 * @brief Implements HttpWriter to buffer and send a response.
 * @date 2024-08-03
 */

#include <stdexcept>
#include <array>
#include <string_view>
#include <string>
#include <sstream>
#include "http1/writer.hpp"

namespace ToyServer::Http1
{
    /* aliases */
    using schemas_t = std::array<std::string_view, static_cast<int>(Schema::last)>;
    using statuses_t = std::array<std::string_view, static_cast<int>(Status::last)>;

    /* constants */

    static constexpr std::size_t writer_buf_len = 2048;

    static constexpr std::string_view text_foo = ""; // placeholder for invalid enums

    static constexpr schemas_t schema_texts = {
        "HTTP/1.0",
        "HTTP/1.1"
    };

    static constexpr statuses_t status_texts = {
        "200 OK",
        "204 No Content",
        "304 Not Modified",
        "400 Bad Request",
        "404 Not Found",
        "415 Unsupported Media",
        "500 Internal Server Error",
        "501 Not Implemented"
    };

    /* helpers impl. */

    std::string_view stringifySchema(Schema schema)
    {
        if (schema >= Schema::last)
            return text_foo;

        return schema_texts.at(static_cast<int>(schema));
    }

    [[nodiscard]] std::string_view stringifyStatus(Status status)
    {
        if (status >= Status::last)
            return text_foo;

        return status_texts.at(static_cast<int>(status));
    }

    /* HttpWriter private impl. */

    void HttpWriter::dumpOutBuffer(std::size_t load_count)
    {
        buf_count = load_count;
        socket->writeFrom(buf_count, out_buf);
        buf_count = 0;
    }

    void HttpWriter::loadChars(const std::string& content)
    {
        // attempt loading of content chars (I also say octets.)
        if (!out_buf.loadChars(content))
        {
            out_buf.clearData();
            throw std::runtime_error {"Output buffer load too big."};
        }
    }

    void HttpWriter::loadChars(const char* octets_ptr, std::size_t len)
    {
        if (!out_buf.loadChars(octets_ptr, len))
        {
            out_buf.clearData();
            throw std::runtime_error {"Output buffer load too big."};
        }
    }

    void HttpWriter::writeLines(const Response& res)
    {
        std::ostringstream sout {};

        for (const auto& [name, value] : res.headers)
            sout << name << ": " << value << "\r\n";

        sout << "\r\n";

        auto data = sout.str();
        loadChars(data);
        dumpOutBuffer(data.length());
    }

    void HttpWriter::writePayload(const Response& res)
    {
        std::size_t res_body_len = res.body.getCapacity();
        loadChars(res.body.getBasePtr(), res_body_len);
        dumpOutBuffer(res_body_len);
    }

    /* HttpWriter public impl. */

    HttpWriter::HttpWriter(ClientSocket* socket_ptr) noexcept
    : out_buf {writer_buf_len}, socket {socket_ptr}, buf_count {0} {}

    void HttpWriter::writeReply(const Response& res)
    {
        writeLines(res);
        writePayload(res);
    }
}
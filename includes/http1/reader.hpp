#ifndef READER_HPP
#define READER_HPP

#include <string>
#include <tuple>
#include <map>
#include "netio/buffers.hpp"
#include "netio/sockets.hpp"
#include "http1/helpers.hpp"
#include "http1/messages.hpp"
#include "uri/parse.hpp"
#include "uri/url.hpp"

namespace ToyServer::Http1
{
    using NetIO::FixedBuffer;
    using NetIO::ClientSocket;

    constexpr bool matchSpacing(char s)
    {
        return s == ' ' || s == '\t';
    }

    struct RawHeader
    {
        std::string name;
        std::string content;
    };

    /**
     * @brief Helper to read and parse a request including its URL string.
     */
    class HttpReader
    {
    private:
        static constexpr std::size_t header_buf_size = 1024;
        static constexpr std::size_t body_buf_size = 4096;

        ToyServer::Uri::UrlParser url_parser;
        FixedBuffer header_buf;
        FixedBuffer body_buf;
        ClientSocket* socket;

        [[nodiscard]] Schema deduceSchema(const std::string& token) const;
        [[nodiscard]] Method deduceMethod(const std::string& token) const;
        [[nodiscard]] Uri::Url parseSimpleURL(const std::string& token);

        [[nodiscard]] std::tuple<Schema, Method, Uri::Url> parseTop();
        [[nodiscard]] RawHeader parseHeader(const std::string& token) const;
        [[nodiscard]] std::map<std::string, std::string> parseHeaders();

        /// @note This actually just reads the body based on `Content-Length`!
        void parseBody(std::size_t content_len = 0);

    public:
        // : url_parser {}, header_buf {header_buf_size}, body_buf {body_buf_size}, socket {} {}
        explicit HttpReader() noexcept;

        HttpReader(const HttpReader& other) = delete;
        HttpReader& operator=(const HttpReader& other) = delete;

        void resetState(ClientSocket* socket) noexcept;

        [[nodiscard]] Request nextRequest();
    };
}

#endif
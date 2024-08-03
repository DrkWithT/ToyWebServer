/**
 * @file reader.cpp
 * @author DrkWithT
 * @brief Implements HTTP request parser / reader.
 * @date 2024-08-02
 */

#include <stdexcept>
#include <sstream>
#include <string>
#include "http1/helpers.hpp"
#include "http1/reader.hpp"

namespace ToyServer::Http1
{
    static constexpr const char* http_1_0_name = "HTTP/1.0";
    static constexpr const char* http_1_1_name = "HTTP/1.1";

    static constexpr const char* http_head_verb = "HEAD";
    static constexpr const char* http_get_verb = "GET";

    static constexpr char http_sp = ' ';
    static constexpr char http_line_end = '\n';

    static constexpr const char* http_content_len_prop = "Content-Length:";

    /* HttpReader private impl. */
    Schema HttpReader::deduceSchema(const std::string& token) const
    {
        if (token == http_1_0_name)
            return Schema::http_1_0;
        else if (token == http_1_1_name)
            return Schema::http_1_1;

        return Schema::http_unknown;
    }

    Method HttpReader::deduceMethod(const std::string& token) const
    {
        if (token == http_head_verb)
            return Method::h1_head;
        else if (token == http_get_verb)
            return Method::h1_get;

        return Method::h1_unknown;
    }

    Uri::Url HttpReader::parseSimpleURL(const std::string& token)
    {
        url_parser.reset(token);

        return url_parser.parseAll();
    }

    std::tuple<Schema, Method, Uri::Url> HttpReader::parseTop()
    {
        std::istringstream str_chop {};

        if (socket.readUntil(http_sp, header_buf) > 0)
            str_chop.str(std::string {header_buf.getBasePtr()});
        else
            throw std::runtime_error {"IOErr: failed to read top request line."};

        // order of line reading: method, path, schema
        std::string token {};

        str_chop >> token;
        Method method = deduceMethod(token);

        token.clear();
        str_chop >> token;
        Uri::Url path = parseSimpleURL(token);

        token.clear();
        str_chop >> token;
        Schema schema = deduceSchema(token);

        header_buf.clearData();
        return {schema, method, path};
    }

    [[nodiscard]] RawHeader HttpReader::parseHeader(const std::string& token) const
    {
        std::istringstream str_chop {token};
        std::string header_name {};
        std::string header_content {};

        str_chop >> header_name;
        str_chop >> header_content;

        return {header_name, header_content};
    };

    std::map<std::string, std::string> HttpReader::parseHeaders()
    {
        std::string temp_line {};
        std::map<std::string, std::string> header_dict {};

        do
        {
            if (socket.readUntil(http_line_end, header_buf) > 0)
                temp_line = std::string {header_buf.getBasePtr()};
            else
                throw std::runtime_error {"IOErr: failed to read header."};

            auto [name, value] = parseHeader(temp_line);

            header_dict[name] = std::move(value);
        }
        while (temp_line.length() > 0);

        return header_dict;
    }

    void HttpReader::parseBody(std::size_t content_len)
    {
        if (content_len == 0)
            return;

        socket.readInto(content_len, body_buf);
    }

    /* HttpReader public impl. */

    HttpReader::HttpReader() noexcept
    : url_parser {}, header_buf {header_buf_size}, body_buf {body_buf_size}, socket {} {}

    void HttpReader::resetState(ClientSocket&& x_socket) noexcept
    {
        socket = std::move(x_socket);
        body_buf.clearData();
        header_buf.clearData();
    }

    Request HttpReader::nextRequest()
    {
        // parse top and its simple URL within
        auto [schema, method, path] = parseTop();

        // read and "parse" headers until blank line by HTTP spec.
        std::map<std::string, std::string> headers = parseHeaders();

        // obey content-length
        std::size_t content_len_value = 0;

        if (headers.contains(http_content_len_prop))
        {
            content_len_value = std::stoul(headers.at(http_content_len_prop));
        }

        // read body at last
        parseBody(content_len_value);

        return {schema, method, path, headers, body_buf};
    }
}
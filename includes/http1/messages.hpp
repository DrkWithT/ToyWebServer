#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <string>
#include <string_view>
#include <map>
#include "netio/buffers.hpp"
#include "http1/helpers.hpp"
#include "uri/url.hpp"

namespace ToyServer::Http1
{
    /**
     * @brief Aggregate representing a simple, non-chunked request.
     */
    struct Request
    {
        Schema schema;
        Method method;
        Uri::Url route;
        std::map<std::string, std::string> headers;
        NetIO::FixedBuffer body;

        Request() noexcept;

        Request(Schema schema_, Method method_, Uri::Url route_, std::map<std::string, std::string> headers_, NetIO::FixedBuffer body_) noexcept;
    };

    /**
     * @brief Aggregate representing a simple, non-chunked response.
     */
    struct Response
    {
        Schema schema;
        Status status;
        std::string_view status_txt;
        std::map<std::string, std::string> headers;
        NetIO::FixedBuffer body;

        Response() noexcept;

        Response(Schema schema_, Status status_, std::string_view status_txt_, std::map<std::string, std::string> headers_, NetIO::FixedBuffer body_) noexcept;
    };
}

#endif
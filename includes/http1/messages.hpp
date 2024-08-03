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
    };

    /**
     * @brief Aggregate representing a simple, non-chunked response.
     */
    struct Response
    {
        Schema schema;
        std::string_view status_txt;
        std::map<std::string, std::string> headers;
        NetIO::FixedBuffer body;
    };
}

#endif
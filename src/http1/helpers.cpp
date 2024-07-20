/**
 * @file helpers.cpp
 * @author DrkWithT
 * @brief Implements HTTP/1.x helpers e.g constants, traits, etc.
 * @date 2024-07-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string_view>
#include "http1/helpers.hpp"

namespace ToyServer::Http1
{
    /// Method helpers

    template <>
    constexpr std::string_view h1_method_v<Method::h1_head> = "HEAD";

    template <>
    constexpr std::string_view h1_method_v<Method::h1_get> = "GET";

    /// Status helpers

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_ok> = "200 OK";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_no_content> = "204 No Content";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_not_modified> = "304 Not Modified";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_bad_request> = "400 Bad Request";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_not_found> = "404 Not Found";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_unsupported_media> = "415 Unsupported Media Type";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_server_err> = "500 Internal Server Error";

    template <>
    constexpr std::string_view h1_status_txt_v<Status::stat_not_implemented> = "501 Not Implemented";
}
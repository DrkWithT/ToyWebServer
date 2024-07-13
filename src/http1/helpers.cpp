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
    constexpr std::string_view h1_status_reason_v<Status::stat_ok> = "OK";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_no_content> = "No Content";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_not_modified> = "Not Modified";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_bad_request> = "Bad Request";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_not_found> = "Not Found";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_unsupported_media> = "Unsupported Media Type";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_server_err> = "Internal Server Error";

    template <>
    constexpr std::string_view h1_status_reason_v<Status::stat_not_implemented> = "Not Implemented";
}
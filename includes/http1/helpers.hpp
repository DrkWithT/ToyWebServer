#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string_view>

namespace ToyServer::Http1
{
    /// HTTP/1.x enums

    /**
     * @brief HTTP 1.x version code
     */
    enum class Schema
    {
        http_1_0,
        http_1_1,
        http_unknown
    };

    /**
     * @brief HTTP verb code
     */
    enum class Method
    {
        h1_head,    // HEAD
        h1_get,     // GET
        h1_unknown  // UNKNOWN
    };

    /**
     * @brief HTTP status codes
     */
    enum class Status
    {
        stat_ok,                // status 200
        stat_no_content,        // status 204
        stat_not_modified,      // status 304
        stat_bad_request,       // status 400
        stat_not_found,         // status 404
        stat_unsupported_media, // status 415
        stat_server_err,        // status 500
        stat_not_implemented,   // status 501
        stat_placeholder = stat_server_err
    };

    /// HTTP/1.x Enum Helpers

    /// @brief Template specialization to convert verbs to strings.
    template <Method M = Method::h1_unknown>
    constexpr std::string_view h1_method_v = "UNKNOWN";

    /// @brief Template specialization to convert status codes to reasons.
    template <Status S = Status::stat_placeholder>
    constexpr std::string_view h1_status_txt_v = "Internal Server Error";
}

#endif
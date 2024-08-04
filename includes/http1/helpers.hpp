#ifndef HELPERS_HPP
#define HELPERS_HPP

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
        http_unknown,
        last = http_unknown
    };

    /**
     * @brief HTTP verb code
     */
    enum class Method
    {
        h1_head,     // HEAD
        h1_get,      // GET
        h1_unknown,
        last = h1_unknown
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
        stat_unknown,
        last = stat_unknown
    };
}

#endif
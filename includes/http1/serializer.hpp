#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "netio/buffers.hpp"
#include "http1/messages.hpp"

namespace ToyServer::Http1
{
    /**
     * @brief Writes server-side response object as stringified data into a `NetIO::FixedBuffer`.
     * 
     * @param buffer The destination buffer.
     * @param request The request object.
     */
    void serializeResponse(NetIO::FixedBuffer& buffer, const Request& request) noexcept;
}

#endif
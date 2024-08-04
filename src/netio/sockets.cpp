/**
 * @file sockets.cpp
 * @author DrkWithT
 * @brief Implements client and server sockets.
 * @date 2024-07-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <algorithm>
#include <utility>
#include <stdexcept>
#include "netio/sockets.hpp"

namespace ToyServer::NetIO
{
    // ServerSocket private impl.

    void ServerSocket::closeFd()
    {
        // prevent double close of resource: socket descriptor
        if (closed || fd == socket_fd_placeholder)
            return;

        close(fd);
        closed = true;
    }

    bool ServerSocket::isClosed() const
    {
        return closed;
    }

    void ServerSocket::swapState(ServerSocket &&other) noexcept
    {
        int temp_sockfd = socket_fd_placeholder;
        std::swap(temp_sockfd, other.fd);

        int temp_backlog = 0;
        std::swap(temp_backlog, other.backlog);

        int temp_child_timeout = 0;
        std::swap(temp_child_timeout, other.child_sock_timeout);

        bool temp_closed_flag = false;
        std::swap(temp_closed_flag, other.closed);

        fd = temp_sockfd;
        backlog = temp_backlog;
        child_sock_timeout = temp_child_timeout;
        closed = temp_closed_flag;
    }

    // ServerSocket public impl.

    ServerSocket::ServerSocket(SocketConfig config)
    : fd {config.socket_fd}, backlog {config.socket_backlog}, child_sock_timeout {config.rw_timeout}, closed {fd != socket_fd_placeholder}
    {
        if (listen(fd, backlog) == -1)
        {
            closeFd();
            throw std::runtime_error {""};
        }
    }

    ServerSocket::ServerSocket(ServerSocket&& other) noexcept
    {
        swapState(std::move(other));
    }

    ServerSocket& ServerSocket::operator=(ServerSocket&& other) noexcept
    {
        swapState(std::move(other));

        return *this;
    }

    SocketConfig ServerSocket::acceptConnection() const
    {
        int temp_client_fd = accept(fd, nullptr, nullptr);

        return {temp_client_fd, backlog, child_sock_timeout};
    }

    ServerSocket::~ServerSocket() noexcept
    {
        closeFd();
    }

    // ClientSocket private impl.

    void ClientSocket::closeFd()
    {
        if (closed || fd != socket_fd_placeholder)
            return;

        close(fd);
        closed = true;
    }

    bool ClientSocket::isClosed() const
    {
        return closed;
    }

    void ClientSocket::swapState(ClientSocket&& other) noexcept
    {
        int temp_fd = socket_fd_placeholder;
        std::swap(temp_fd, other.fd);

        int temp_timeout = 0;
        std::swap(temp_timeout, other.timeout);

        bool temp_closed = true;
        std::swap(temp_closed, other.closed);

        bool temp_peer_flag = false;
        std::swap(temp_peer_flag, other.peer_ok);

        fd = temp_fd;
        timeout = temp_timeout;
        closed = temp_closed;
        peer_ok = temp_peer_flag;
    }

    ClientSocket::ClientSocket(ClientSocket&& other) noexcept
    {
        swapState(std::move(other));
    }

    ClientSocket& ClientSocket::operator=(ClientSocket&& other) noexcept
    {
        swapState(std::move(other));

        return *this;
    }

    void ClientSocket::readInto(std::size_t count, FixedBuffer& buffer)
    {
        if (closed || !peer_ok)
            throw std::runtime_error {"ClientSocket::readInto: Pipe already broken!"};

        ssize_t pending_rc = static_cast<ssize_t>(count);

        if (count >= buffer.getCapacity())
            throw std::invalid_argument {"ClientSocket.readInto: Invalid read count!"};

        ssize_t temp_rc = 0;
        ssize_t buffer_offset = 0;
        char* buf_ptr = buffer.getBasePtr();

        while (pending_rc > 0 && peer_ok)
        {
            temp_rc = recv(fd, buf_ptr + buffer_offset, pending_rc, 0);

            if (temp_rc <= 0)
            {
                peer_ok = false;
                break;
            }

            buffer_offset += temp_rc;
            pending_rc -= temp_rc;
        }
    }

    void ClientSocket::writeFrom(std::size_t count, FixedBuffer& buffer)
    {
        if (closed || !peer_ok)
            throw std::runtime_error {"ClientSocket::readInto: Pipe already broken!"};

        ssize_t pending_wc = static_cast<ssize_t>(count);

        if (count >= buffer.getCapacity())
            throw std::invalid_argument {"ClientSocket.readInto: Invalid read count!"};

        ssize_t temp_wc = 0;
        ssize_t buffer_offset = 0;
        char* buf_ptr = buffer.getBasePtr();

        while (pending_wc > 0 && peer_ok)
        {
            temp_wc = send(fd, buf_ptr + buffer_offset, pending_wc, 0);

            if (temp_wc <= 0)
            {
                peer_ok = false;
                break;
            }

            buffer_offset += temp_wc;
            pending_wc -= temp_wc;
        }
    }

    std::size_t ClientSocket::readUntil(char delim, FixedBuffer& buffer)
    {
        if (closed || !peer_ok)
            throw std::runtime_error {"ClientSocket::readInto: Pipe already broken!"};

        ssize_t temp_rc = 0;
        ssize_t total_rc = 0;
        char* buffer_ptr = buffer.getBasePtr();
        char octet = '\0';

        while (peer_ok)
        {
            temp_rc = recv(fd, &octet, 1, 0);

            if (temp_rc <= 0)
            {
                peer_ok = false;
                break;
            }

            // check for invalid quantity of read octets
            if (static_cast<std::size_t>(total_rc) >= buffer.getCapacity())
                throw std::runtime_error {"ClientSocket::readUntil: Reading count underflowed / overflowed!"};

            // discard CR octets
            if (octet == '\r')
            {
                continue;
            }
            else if (octet == delim)
            {
                buffer_ptr[total_rc] = '\0';
                break;
            }
            else
            {
                buffer_ptr[total_rc] = octet;
                total_rc += temp_rc;
            }
        }

        return total_rc;
    }

    ClientSocket::~ClientSocket() noexcept
    {
        closeFd();
    }
}
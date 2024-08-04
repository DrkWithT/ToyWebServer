#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include <sys/socket.h>
#include "netio/buffers.hpp"
#include "netio/config.hpp"

namespace ToyServer::NetIO
{
    /**
     * @brief RAII wrapper for a listening socket's state.
     */
    class ServerSocket
    {
    private:
        static constexpr int socket_fd_placeholder = -1; // invalid socket fd, placeholder only!

        int fd;
        int backlog;
        int child_sock_timeout;
        bool closed;

        void closeFd();
        [[nodiscard]] bool isClosed() const;
        void swapState(ServerSocket&& other) noexcept;

    public:
        constexpr ServerSocket()
        : fd {socket_fd_placeholder}, backlog {0}, child_sock_timeout {0}, closed {true} {}

        ServerSocket(SocketConfig config);

        ServerSocket(const ServerSocket& other) = delete;
        ServerSocket& operator=(const ServerSocket& other) = delete;

        ServerSocket(ServerSocket&& other) noexcept;
        ServerSocket& operator=(ServerSocket&& other) noexcept;

        [[nodiscard]] SocketConfig acceptConnection() const;

        ~ServerSocket() noexcept;
    };

    /**
     * @brief RAII wrapper for a client socket's state.
     */
    class ClientSocket
    {
    private:
        static constexpr int socket_fd_placeholder = -1; // invalid socket fd, placeholder only!

        int fd;
        int timeout;
        bool closed;
        bool peer_ok;

        void closeFd();
        [[nodiscard]] bool isClosed() const;
        void swapState(ClientSocket&& other) noexcept;

    public:
        constexpr ClientSocket()
        : fd {socket_fd_placeholder}, timeout {0}, closed {true}, peer_ok {false} {}

        constexpr ClientSocket(SocketConfig config)
        : fd {config.socket_fd}, timeout {config.rw_timeout}, closed {fd != socket_fd_placeholder}, peer_ok {true}
        {
            struct linger timeout_opts {};
            timeout_opts.l_linger = timeout;
            timeout_opts.l_onoff = 1;

            setsockopt(fd, SOL_SOCKET, SO_LINGER, &timeout_opts, sizeof(timeout_opts));
        }

        ClientSocket(const ClientSocket& other) = delete;
        ClientSocket& operator=(const ClientSocket& other) = delete;

        ClientSocket(ClientSocket&& other) noexcept;
        ClientSocket& operator=(ClientSocket&& other) noexcept;

        void readInto(std::size_t count, FixedBuffer& buffer);
        void writeFrom(std::size_t count, FixedBuffer& buffer);
        [[nodiscard]] std::size_t readUntil(char delim, FixedBuffer& buffer);

        ~ClientSocket() noexcept;
    };
}

#endif
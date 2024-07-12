#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <stdexcept>
#include <optional>

namespace ToyServer::NetIO
{
    // Utility decls.

    static constexpr int socket_timeout_default = 10000; // timeout in ms

    /**
     * @brief Simple aggregate holding important options for setting up an AddrInfo wrapper.
     */
    struct SocketHints
    {
        std::string socket_port_str;
        int socket_backlog_num;
        int socket_timeout_num;

        constexpr SocketHints(const char* port_cstr, int backlog, int timeout) noexcept
        : socket_port_str {port_cstr}, socket_backlog_num {backlog}, socket_timeout_num {timeout} {}
    };

    /**
     * @brief Simple aggregate holding important option values for socket creation.
     */
    struct SocketConfig
    {
        int socket_fd; // sockfd : int

        int address_family; // ai_family : int

        int socket_type; // ai_socktype : int

        int transport_protocol; // ai_protocol : int

        int rw_timeout; // SO_RECVTIMEO : int

        constexpr SocketConfig(int fd, int addr_family, int sock_type, int layer_protocol, int io_timeout)
        : socket_fd {fd}, address_family {addr_family}, socket_type {sock_type}, transport_protocol {layer_protocol}, rw_timeout {io_timeout} {}
    };

    /**
     * @brief RAII wrapper for getaddrinfo raw result list. The intrusive list will be destroyed when an instance of `AddrInfo` expires in any way.
     */
    class AddrInfo
    {
    private:
        struct addrinfo* opt_head;
        struct addrinfo* opt_cursor;
        int so_backlog;
        int so_timeout;

        [[nodiscard]] bool atEnd() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;
        void advanceCursor() noexcept;

    public:
        AddrInfo(const SocketHints& hints);

        AddrInfo(const AddrInfo& other) = delete;
        AddrInfo& operator=(const AddrInfo& other) = delete;

        std::optional<SocketConfig> getNextOption() noexcept;

        ~AddrInfo() noexcept;
    };
}

#endif
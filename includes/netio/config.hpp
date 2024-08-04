#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <stdexcept>
#include <optional>

namespace ToyServer::NetIO
{
    // Utility decls.

    enum class SocketConfigTag
    {
        server,
        client,
        unknown
    };

    /**
     * @brief Simple aggregate holding important options for setting up an AddrInfo wrapper.
     */
    struct SocketHints
    {
        std::string socket_port_str;   // port number as text
        int socket_backlog_num;        // count of pending connections
        int socket_timeout_num;        // seconds

        constexpr SocketHints(const char* port_cstr, int backlog, int timeout) noexcept
        : socket_port_str {port_cstr}, socket_backlog_num {backlog}, socket_timeout_num {timeout} {}
    };

    /**
     * @brief Simple aggregate holding important option values for server socket creation.
     */
    struct SocketConfig
    {
        int socket_fd; // sockfd : int
        int socket_backlog; // backlog : int
        int rw_timeout; // SO_LINGER : int
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
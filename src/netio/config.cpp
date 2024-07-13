/**
 * @file config.cpp
 * @author DrkWithT
 * @brief Implements wrappers for BSD addrinfo API. 
 * @date 2024-07-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

#include "netio/config.hpp"

namespace ToyServer::NetIO
{
    // AddrInfo private impl.

    bool AddrInfo::atEnd() const noexcept { return opt_cursor == nullptr; }

    bool AddrInfo::isEmpty() const noexcept { return opt_head == nullptr; }

    void AddrInfo::advanceCursor() noexcept
    {
        if (atEnd())
            return;

        opt_cursor = opt_cursor->ai_next;
    }

    // AddrInfo public impl.

    AddrInfo::AddrInfo(const SocketHints& hints)
    {
        auto [port_sv, backlog, timeout] = hints;

        struct addrinfo pre_hints;
        std::memset(&pre_hints, 0, sizeof(pre_hints));
        pre_hints.ai_family = AF_UNSPEC;
        pre_hints.ai_socktype = SOCK_STREAM;
        pre_hints.ai_flags = AI_PASSIVE;

        int err_code = 0;

        if ((err_code = getaddrinfo(nullptr, port_sv.c_str(), &pre_hints, &opt_head)) != 0)
            throw std::runtime_error {std::string {gai_strerror(err_code)}};

        so_backlog = backlog;
        so_timeout = timeout;
        this->opt_cursor = opt_head;
    }

    std::optional<SocketConfig> AddrInfo::getNextOption() noexcept
    {
        if (isEmpty() || atEnd())
            return {};

        const struct addrinfo* temp = opt_cursor;

        int family = temp->ai_family;
        int socktype = temp->ai_socktype;
        int protocol = temp->ai_protocol;
        int sockfd = socket(family, socktype, protocol);
        int timeout = so_timeout;

        if (bind(sockfd, temp->ai_addr, temp->ai_addrlen) == -1)
        {
            close(sockfd);
            sockfd = -1;
        }

        advanceCursor();

        return std::optional {SocketConfig {sockfd, .socket_backlog=so_backlog, timeout}};
    }

    AddrInfo::~AddrInfo() noexcept
    {
        if (isEmpty())
            return;

        freeaddrinfo(opt_head);
        opt_cursor = nullptr;
    }
}
#pragma once

#include "socket.h"

#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <print>
#include <format>


namespace server_utils
{

using socketFd = int;


template <typename... Args>
void LogErr(std::format_string<Args...> fmt_str, Args&&... args)
{
    int saved_errno = errno;
    std::println(stderr, "{} : {}",
                 std::format(fmt_str, std::forward<Args>(args)...),
                 std::strerror(saved_errno));
}

Socket MakeTCPServerAndListen(uint16_t port = 8080)
{
    Socket server_sock (socket(AF_INET, SOCK_STREAM, 0));
    if (!server_sock.is_valid())
    {
        LogErr("[server] failed to create socket");
        return {};
    }

    sockaddr_in server_addr{};
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    constexpr int reuse_enabled = 1;
    setsockopt(server_sock.fd, SOL_SOCKET, SO_REUSEADDR, &reuse_enabled, sizeof(reuse_enabled));

    if (bind(server_sock.fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        LogErr("[server] failed to bind socket on port: {}", port);
        return {};
    }
    if (listen(server_sock.fd, SOMAXCONN) < 0)
    {
        LogErr("[server] failed to listen on port: {}", port);
        return {};
    }

    return server_sock;
}

Socket ConnectToServer(std::string_view server_ip, uint16_t port)
{
    Socket client_sock(socket(AF_INET, SOCK_STREAM, 0));
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    inet_pton(AF_INET, server_ip.data(), &server_addr.sin_addr);
    if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        LogErr("[client] failed to connect to server on port: {}", port);
        return {};
    }
}

Socket AcceptClient(Socket& server_sock)
{
    return Socket(accept(server_sock.fd, nullptr, nullptr));
}


} // namespace server_utils

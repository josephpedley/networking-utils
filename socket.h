#pragma once

/**
 * @brief RAII owner of a socket file descriptor.
 *
 * Closes the fd when Socket goes out of scope. An fd of -1 means there's no socket
 * (default constructed, moved from object, or a failed creation)
 */
#include <utility>
struct Socket 
{
    int fd = -1; ///< Owned file descriptor, -1 when invalid.

    Socket() = default;

    /// @brief Takes ownership of a socket fd. 
    explicit Socket(int file_descriptor)
        : fd(file_descriptor)
    { }

    ~Socket()
    {
        if (fd >= 0)
            close(fd);
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    /// @brief Transfers ownership of a socket
    Socket(Socket&& other) noexcept
        : fd(std::exchange(other.fd,  -1))
    { }

    /// @brief Closes the current fd, then takes ownership of the source's fd.
    Socket& operator=(Socket&& other) noexcept
    {
        if (this != &other)
        {
            if (fd >= 0)
                close(fd);
            
            fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    bool is_valid() const { return fd >= 0; }
};

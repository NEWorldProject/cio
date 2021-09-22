#pragma once

#include <cstdint>
#include <exception>

namespace cio {
    enum status {
        CIO_OK,
        CIO_EACCES, //permission denied
        CIO_EADDRINUSE, //address already in use
        CIO_EADDRNOTAVAIL, //address not available
        CIO_EAFNOSUPPORT, // address family not supported
        CIO_EAGAIN, // resource temporarily unavailable
        CIO_EALREADY, // connection already in progress
        CIO_EBADF, // bad file descriptor
        CIO_EBUSY, // resource busy or locked
        CIO_ECANCELED, // operation canceled
        CIO_ECHARSET, // invalid Unicode character
        CIO_ECONNABORTED, //software caused connection abort
        CIO_ECONNREFUSED, //connection refused
        CIO_ECONNRESET, //connection reset by peer
        CIO_EDESTADDRREQ, //destination address required
        CIO_EEXIST, // file already exists
        CIO_EFAULT, //bad address in system call argument
        CIO_E2BIG, // file too large
        CIO_EHOSTUNREACH, // host is unreachable
        CIO_EINTR, // interrupted system call
        CIO_EINVAL, // invalid argument
        CIO_EIO, // i/o error
        CIO_EISCONN, // socket is already connected
        CIO_EISDIR, // illegal operation on a directory
        CIO_ELOOP, // too many symbolic links encountered
        CIO_EMFILE, // too many open files
        CIO_EMSGSIZE, // message too long
        CIO_ENAMETOOLONG, // name too long
        CIO_ENETDOWN, // network is down
        CIO_ENETUNREACH, // network is unreachable
        CIO_ENFILE, // file table overflow
        CIO_ENOBUFS, // no buffer space available
        CIO_ENODEV, // no such device
        CIO_ENOENT, // no such file or directory
        CIO_ENOMEM, // not enough memory
        CIO_ENONET, // machine is not on the network
        CIO_ENOPROTOOPT, // protocol not available
        CIO_ENOSPC, // no space left on device
        CIO_ENOSYS, // function not implemented
        CIO_ENOTCONN, // socket is not connected
        CIO_ENOTDIR, // not a directory
        CIO_ENOTEMPTY, // directory not empty
        CIO_ENOTSOCK, // socket operation on non-socket
        CIO_ENOTSUP, // operation not supported on socket
        CIO_EOVERFLOW, // value too large for defined data type
        CIO_EPERM, // operation not permitted
        CIO_EPIPE, // broken pipe
        CIO_EPROTO, // protocol error
        CIO_EPROTONOSUPPORT, // protocol not supported
        CIO_EPROTOTYPE, // protocol wrong type for socket
        CIO_ERANGE, // result too large
        CIO_EROFS, // read-only file system
        CIO_ESHUTDOWN, // cannot send after transport endpoint shutdown
        CIO_ESPIPE, // invalid seek
        CIO_ESRCH, // no such process
        CIO_ETIMEDOUT, // connection timed out
        CIO_ETXTBSY, // text file is busy
        CIO_EXDEV, // cross-device link not permitted
        CIO_UNKNOWN, // unknown error
        CIO_EOF, // end of file
        CIO_ENXIO, // no such device or address
        CIO_EMLINK, // too many links
        CIO_ENOTTY, // inappropriate ioctl for device
        CIO_EFTYPE, // inappropriate file type or format
        CIO_EILSEQ, // illegal byte sequence
        CIO_ESOCKTNOSUPPORT // socket type not supported
    };

    struct exception_errc : std::exception {
        status errc;

        explicit exception_errc(const status status) noexcept: errc(status) {}
    };

    using completion_callback = void (*)(uint32_t id, uint32_t status, uint32_t completed_bytes) noexcept;

    void set_completion_callback(completion_callback callback) noexcept;

    // Operations on datagram handles (e.g. UDP socket)
    void datagram_send(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

    void datagram_receive(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

    void datagram_send_multi(uint64_t hdc, uint32_t id, uint64_t *buffers, uint64_t *sizes);

    void datagram_receive_multi(uint64_t hdc, uint32_t id, uint64_t *buffers, uint64_t *sizes);
}

// Operations on block devices (e.g. Regular File)
namespace cio::block {
    enum flag {
        O_READ = 1ul,
        O_WRITE = 2ul,
        O_CREAT = 4ul,
        O_EXCL = 8ul,
        O_TRUNC = 16ul,
        O_EXLOCK = 32ul
    };

    uint64_t open(const char *path_utf8, uint32_t flags);

    void close(uint64_t hdc) noexcept;

    void read(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset) noexcept;

    void write(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset) noexcept;

    void readv(uint64_t hdc, uint32_t id, uint64_t *buffers, uint64_t *sizes, uint64_t *offsets, uint64_t *spans);

    void writev(uint64_t hdc, uint32_t id, uint64_t *buffers, uint64_t *sizes, uint64_t *offsets, uint64_t *spans);
}

// Operations on stream handles (e.g. TCP socket)
namespace cio::stream {
    void connect_tcp(uint32_t id, const char* address, int port);

    void close(uint64_t hdc);

    void send(uint64_t hdc, uint32_t id, uint64_t buffer, uint32_t size) noexcept;

    void recv(uint64_t hdc, uint32_t id, uint64_t buffer, uint32_t size) noexcept;

    void sendv(uint64_t hdc, uint32_t id, uint32_t count, uint64_t *buffers, uint32_t *sizes) noexcept;

    void recvv(uint64_t hdc, uint32_t id, uint32_t count, uint64_t *buffers, uint32_t *sizes) noexcept;
}
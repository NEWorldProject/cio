#pragma once

enum cio_status {
    CIO_OK,
    CIO_PARTIAL,
    CIO_EACCES, //permission denied
    CIO_EADDRINUSE, //address already in use
    CIO_EADDRNOTAVAIL, //address not available
    CIO_EAFNOSUPPORT, // address family not supported
    CIO_EAGAIN, // resource temporarily unavailable
    CIO_EAI_ADDRFAMILY, //address family not supported
    CIO_EAI_AGAIN, // temporary failure
    CIO_EAI_BADFLAGS, // bad ai_flags value
    CIO_EAI_BADHINTS, // invalid value for hints
    CIO_EAI_CANCELED, // request canceled
    CIO_EAI_FAIL, // permanent failure
    CIO_EAI_FAMILY, // ai_family not supported
    CIO_EAI_MEMORY, // out of memory
    CIO_EAI_NODATA, // no address
    CIO_EAI_NONAME, // unknown node or service
    CIO_EAI_OVERFLOW, // argument buffer overflow
    CIO_EAI_PROTOCOL, // resolved protocol is unknown
    CIO_EAI_SERVICE, // service not available for socket type
    CIO_EAI_SOCKTYPE, // socket type not supported
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
    CIO_EFBIG, // file too large
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

struct cio_result {
    uint32_t id{};
    uint32_t status{};
    uint64_t completed_bytes{};
};

// Feedback Bus
void cio_trap_worker();

// Operations on block devices (e.g. Regular File)
void cio_block_open();

void cio_block_close();

void cio_block_read(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset);

void cio_block_write(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset);

void cio_block_read_multi(
        uint32_t hdc, uint32_t id,
        uint64_t *buffers, uint64_t *sizes,
        uint64_t *offsets, uint64_t *spans
);

void cio_block_write_multi(
        uint32_t hdc, uint32_t id,
        uint64_t *buffers, uint64_t *sizes,
        uint64_t *offsets, uint64_t *spans
);

// Operations on stream handles (e.g. TCP socket)
void cio_stream_send(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

void cio_stream_receive(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

void cio_stream_send_multi(uint32_t hdc, uint32_t id,uint64_t *buffers, uint64_t *sizes);

void cio_stream_receive_multi(uint32_t hdc, uint32_t id,uint64_t *buffers, uint64_t *sizes);

// Operations on datagram handles (e.g. UDP socket)
void cio_datagram_send(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

void cio_datagram_receive(uint32_t hdc, uint32_t id, uint64_t buffer, uint64_t size);

void cio_datagram_send_multi(uint32_t hdc, uint32_t id,uint64_t *buffers, uint64_t *sizes);

void cio_datagram_receive_multi(uint32_t hdc, uint32_t id,uint64_t *buffers, uint64_t *sizes);
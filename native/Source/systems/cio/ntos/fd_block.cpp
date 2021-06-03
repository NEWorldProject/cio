#include <array>
#include <deque>
#include <string>
#include <vector>
#include "iocp.hpp"
#include "error.hpp"

using namespace cio;
using namespace cio::block;

namespace {
    template<class T> using temp_basic_string = std::basic_string<T, std::char_traits<T>, temp_alloc<T>>;
    using temp_u16string = temp_basic_string<char16_t>;

    temp_u16string ntos_get_path(const char *path_utf8) noexcept {
        std::string_view path_view{path_utf8};
        auto path_wide = std::vector<char16_t, temp_alloc<char16_t>>();
        MultiByteToWideChar(
                CP_UTF8, MB_COMPOSITE,
                path_view.data(), static_cast<int>(path_view.size()),
                reinterpret_cast<LPWSTR>(path_wide.data()), static_cast<int>(path_wide.capacity())
        );
        return temp_u16string(uR"(\\?\)") + path_wide.data();
    }

    DWORD ntos_file_make_access(uint32_t flags) {
        const auto read = flags & O_READ;
        const auto write = flags & O_WRITE;
        if (!read && !write) throw exception_errc(CIO_EACCES);
        DWORD result = 0ul;
        if (read) result |= GENERIC_READ;
        if (write) result |= GENERIC_WRITE;
        return result;
    }

    DWORD ntos_file_make_creation_disposition(uint32_t flags) noexcept {
        DWORD disposition{};
        switch (flags & (O_CREAT | O_EXCL | O_TRUNC)) {
            case 0ul:
            case O_EXCL:
                disposition = OPEN_EXISTING;
                break;
            case O_CREAT:
                disposition = OPEN_ALWAYS;
                break;
            case O_CREAT | O_EXCL:
            case O_CREAT | O_TRUNC | O_EXCL:
                disposition = CREATE_NEW;
                break;
            case O_TRUNC:
            case O_TRUNC | O_EXCL:
                disposition = TRUNCATE_EXISTING;
                break;
            case O_CREAT | O_TRUNC:
                disposition = CREATE_ALWAYS;
                break;
            default:;
        }
        return disposition;
    }

    DWORD ntos_file_make_share(uint32_t flags) noexcept {
        return ((flags & O_EXLOCK) ? 0u : (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE));
    }

    uint64_t ntos_create_file(const char *path_utf8, uint32_t flags) {
        const auto path = ntos_get_path(path_utf8);
        const auto hFile = CreateFileW(
                reinterpret_cast<LPCWSTR>(path.c_str()),
                ntos_file_make_access(flags),
                ntos_file_make_share(flags),
                nullptr,
                ntos_file_make_creation_disposition(flags),
                FILE_FLAG_OVERLAPPED | FILE_FLAG_WRITE_THROUGH,
                nullptr
        );
        if (hFile == INVALID_HANDLE_VALUE) {
            const auto error = GetLastError();
            if (error == ERROR_FILE_EXISTS && (flags & O_CREAT) && !(flags & O_EXCL))
                throw exception_errc(CIO_EISDIR);
            else
                throw exception_errc(internal::translate_sys_error(error));
        }
        const auto ret = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(hFile));
        try {
            internal::iocp_register(hFile, ret);
        }
        catch (...) {
            CloseHandle(hFile);
            throw;
        }
        return ret;
    }

    void ntos_read(uint64_t hdc, uint64_t buffer, uint64_t size, internal::ntos_simple_aio_record *aio) {
        auto result = ReadFile(
                reinterpret_cast<HANDLE>(static_cast<uintptr_t>(hdc)),
                reinterpret_cast<LPVOID>(static_cast<uintptr_t>(buffer)),
                static_cast<DWORD>(size),
                nullptr,
                &aio->overlapped
        );
        if (result == TRUE) return ntos_release_overlapped(aio, size);
        if (GetLastError() != ERROR_IO_PENDING) return ntos_release_overlapped(aio, 0);
    }

    void ntos_write(uint64_t hdc, uint64_t buffer, uint64_t size, internal::ntos_simple_aio_record *aio) {
        auto result = WriteFile(
                reinterpret_cast<HANDLE>(static_cast<uintptr_t>(hdc)),
                reinterpret_cast<LPVOID>(static_cast<uintptr_t>(buffer)),
                static_cast<DWORD>(size),
                nullptr,
                &aio->overlapped
        );
        if (result == TRUE) return ntos_release_overlapped(aio, size);
        if (GetLastError() != ERROR_IO_PENDING) return ntos_release_overlapped(aio, 0);
    }

    template<class T>
    using temp_deque = std::deque<T, temp_alloc<T>>;

    temp_deque<std::array<uint64_t, 3>> splice_complex_request(
            uint64_t *buffers, uint64_t *sizes,
            uint64_t *offsets, uint64_t *spans
    ) {
        auto result = temp_deque<std::array<uint64_t, 3>>();
        uint64_t offset = *offsets, span = *spans, head = *buffers, size = *sizes;
        for (;;) {
            while (size > 0) {
                const auto slice = std::min(size, span);
                result.push_back({head, offset, slice});
                head += slice, size -= slice, offset += slice, span -= slice;
                if (span == 0) (offset = *(++offsets), span = *(++spans));
                if (span == 0) return result;
            }
            head = *(++buffers), size = *(++sizes);
            if (size == 0) throw exception_errc(CIO_EINVAL);
        }
    }
}

uint64_t cio::block::open(const char *path_utf8, uint32_t flags) {
    return ntos_create_file(path_utf8, flags);
}

void cio::block::close(uint64_t hdc) noexcept {
    CloseHandle(reinterpret_cast<HANDLE>(static_cast<uintptr_t>(hdc)));
}

void cio::block::read(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset) noexcept {
    const auto aio = internal::ntos_alloc_simple_aio(id);
    aio->set_overlapped(offset);
    ntos_read(hdc, buffer, size, aio);
}

void cio::block::write(uint64_t hdc, uint32_t id, uint64_t buffer, uint64_t size, uint64_t offset) noexcept {
    const auto aio = internal::ntos_alloc_simple_aio(id);
    aio->set_overlapped(offset);
    ntos_write(hdc, buffer, size, aio);
}

void cio::block::read_multi(
        uint64_t hdc, uint32_t id,
        uint64_t *buffers, uint64_t *sizes,
        uint64_t *offsets, uint64_t *spans
) {
    const auto slices = splice_complex_request(buffers, sizes, offsets, spans);
    const auto aio = internal::ntos_alloc_complex_aio(id, static_cast<int32_t>(slices.size()));
    auto iter = aio->children;
    for (auto &&[buffer, offset, size] : slices) {
        iter->set_overlapped(offset);
        ntos_read(hdc, buffer, size, iter++);
    }
}

void cio::block::write_multi(
        uint64_t hdc, uint32_t id,
        uint64_t *buffers, uint64_t *sizes,
        uint64_t *offsets, uint64_t *spans
) {
    const auto slices = splice_complex_request(buffers, sizes, offsets, spans);
    const auto aio = internal::ntos_alloc_complex_aio(id, static_cast<int32_t>(slices.size()));
    auto iter = aio->children;
    for (auto &&[buffer, offset, size] : slices) {
        iter->set_overlapped(offset);
        ntos_write(hdc, buffer, size, iter++);
    }
}
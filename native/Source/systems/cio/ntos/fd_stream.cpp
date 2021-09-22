#define UNICODE

#include <array>
#include <vector>
#include <charconv>
#include "iocp.hpp"
#include <ws2tcpip.h>
#include <mswsock.h>

using namespace cio;
using namespace cio::stream;
using namespace internal;

namespace {
    GUID g_id_connect_ex = WSAID_CONNECTEX;

    class state_machine {
        static temp_alloc<state_machine> ig_self_alloc;
    public:
        [[maybe_unused]] static void start(uint32_t id, temp_u16string address, int port) {
            auto st = new(ig_self_alloc.allocate(1)) state_machine(id, std::move(address), port);
            st->step0();
        }

    private:
        WSAOVERLAPPED m_overlap{0, 0, 0, 0, nullptr};
        uint32_t m_id;
        temp_u16string m_address, m_port{};
        ADDRINFOEXW m_hint, *m_result{};
        SOCKET m_socket{};

        state_machine(uint32_t id, temp_u16string address, int port) noexcept:
                m_id{id}, m_address{std::move(address)},
                m_hint{0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, nullptr, nullptr, nullptr, 0, nullptr, nullptr} {
            char buf[16] = {0};
            m_port = temp_u16string(buf, std::to_chars(buf, buf + 16, port).ptr);
        }

        // Extract information from address, with potential DNS request
        void step0() noexcept {
            const auto result = GetAddrInfoExW(
                    reinterpret_cast<PCWSTR>(m_address.c_str()),
                    reinterpret_cast<PCWSTR>(m_port.c_str()),
                    NS_ALL, nullptr,
                    &m_hint, &m_result, nullptr, &m_overlap,
                    [](DWORD error, DWORD, LPWSAOVERLAPPED ov) noexcept {
                        reinterpret_cast<state_machine *>(ov)->step0_next(static_cast<int>(error));
                    },
                    nullptr
            );
            if (result != WSA_IO_PENDING) step0_next(result);
        };

        void step0_next(int code) noexcept { if (code == ERROR_SUCCESS) return step1(); else step0_error(code); }

        void step0_error(int error) noexcept {
            ntos_release_aio(m_id, error);
            step0_cleanup();
        }

        void step0_cleanup() noexcept {
            this->~state_machine();
            ig_self_alloc.deallocate(this, 1);
        }

        // Create the new socket
        void step1() noexcept {
            m_socket = WSASocketW(
                    m_result->ai_family, m_result->ai_socktype,
                    m_result->ai_protocol, nullptr, 0, WSA_FLAG_OVERLAPPED
            );
            if (m_socket == INVALID_SOCKET) return step1_error(WSAGetLastError()); else return step2();
        }

        void step1_error(int error) noexcept {
            ntos_release_aio(m_id, error);
            step1_cleanup();
        }

        void step1_cleanup() noexcept {
            FreeAddrInfoExW(m_result);
            step0_cleanup();
        }

        // Register socket with IOCP
        void step2() noexcept {
            try {
                ntos_iocp_register((HANDLE) m_socket);
                step3();
            }
            catch (exception_errc &e) {
                ntos_release_aio(m_id, e);
                step2_cleanup();
            }
        }

        void step2_cleanup() noexcept {
            closesocket(m_socket);
            step1_cleanup();
        }

        // Asynchronously connect
        void step3() noexcept {
            auto overlap = ntos_bind_iocp_record([this](DWORD e, DWORD) noexcept { step3_next(e); });
            auto result = connect(m_result->ai_addr, static_cast<int>(m_result->ai_addrlen), overlap);
            if (result != ERROR_IO_PENDING) return ntos_iocp_use_record(result, 0, overlap);
        }

        void step3_next(DWORD e) noexcept {
            ntos_release_aio(m_id, static_cast<int>(e), (e == ERROR_SUCCESS) ? m_socket : 0);
            step2_cleanup();
        }

        int connect(const struct sockaddr *name, int name_len, LPOVERLAPPED ov) const noexcept {
            LPFN_CONNECTEX func{};
            DWORD dwBytes{};
            const auto ret = WSAIoctl(
                    m_socket,
                    SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &g_id_connect_ex, sizeof(g_id_connect_ex), &func, sizeof(func),
                    &dwBytes, nullptr, nullptr
            );
            if (ret == SOCKET_ERROR) return WSAGetLastError();
            const auto ret2 = func(m_socket, name, name_len, nullptr, 0, nullptr, ov);
            if (ret2 == TRUE) return ERROR_SUCCESS; else return WSAGetLastError();
        }
    };

    temp_alloc<state_machine> state_machine::ig_self_alloc;

    temp_u16string ntos_get_address(const char *address) noexcept {
        std::string_view path_view{address};
        auto path_wide = std::vector<char16_t, temp_alloc<char16_t>>(path_view.length() + 2);
        path_wide[MultiByteToWideChar(
                CP_UTF8, MB_COMPOSITE,
                path_view.data(), static_cast<int>(path_view.size()),
                reinterpret_cast<LPWSTR>(path_wide.data()), static_cast<int>(path_wide.capacity())
        )] = 0;
        return temp_u16string(path_wide.data());
    }

    struct ntos_net_bind final {
    private:
        struct buf_del {
            size_t n;

            void operator()(WSABUF *x) const noexcept { alloc_buf.deallocate(x, n); }
        };

        inline static temp_alloc<WSABUF> alloc_buf;
    public:
        uint32_t id;
        std::unique_ptr<WSABUF[], buf_del> buf;

        explicit ntos_net_bind(uint32_t id, uint32_t length, void *buffer) noexcept:
                id{id}, buf{alloc_buf.allocate(1), {1}} {
            buf[0] = WSABUF{length, static_cast<CHAR *>(buffer)};
        }

        explicit ntos_net_bind(uint32_t id, uint32_t count, uint32_t *lengths, void **buffers) noexcept:
                id{id}, buf{alloc_buf.allocate(count), {count}} {
            for (int i = 0; i < count; ++i) buf[i] = WSABUF{lengths[i], static_cast<CHAR *>(buffers[i])};
        }

        // TODO: double check
        void operator()(DWORD e, DWORD c) const noexcept { ntos_release_aio(id, static_cast<int>(e), c); }
    };

    void ntos_net_handle_error(LPOVERLAPPED ov) noexcept {
        const auto error = WSAGetLastError();
        if (error != WSA_IO_PENDING) ntos_iocp_use_record(error, 0, ov);
    }

    void ntos_net_send(uint64_t hdc, ntos_net_bind &&bind) noexcept {
        const auto buf = bind.buf.get();
        const auto ov = ntos_bind_iocp_record(std::move(bind));
        const auto result = WSASend(hdc, buf, 1, nullptr, 0, ov, nullptr);
        if (result != 0) ntos_net_handle_error(ov);
    }

    void ntos_net_recv(uint64_t hdc, ntos_net_bind &&bind) noexcept {
        const auto buf = bind.buf.get();
        const auto ov = ntos_bind_iocp_record(std::move(bind));
        const auto result = WSARecv(hdc, buf, 1, nullptr, nullptr, ov, nullptr);
        if (result != 0) ntos_net_handle_error(ov);
    }
}

void cio::stream::connect_tcp(uint32_t id, const char *address, int port) {
    state_machine::start(id, ntos_get_address(address), port);
}

void cio::stream::close(uint64_t hdc) {
    closesocket(hdc);
}

void cio::stream::send(uint64_t hdc, uint32_t id, uint64_t buffer, uint32_t size) noexcept {
    ntos_net_send(hdc, ntos_net_bind(id, size, reinterpret_cast<void *>(buffer)));
}

void cio::stream::recv(uint64_t hdc, uint32_t id, uint64_t buffer, uint32_t size) noexcept {
    ntos_net_recv(hdc, ntos_net_bind(id, size, reinterpret_cast<void *>(buffer)));
}

void cio::stream::sendv(uint64_t hdc, uint32_t id, uint32_t count, uint64_t *buffers, uint32_t *sizes) noexcept {
    ntos_net_send(hdc, ntos_net_bind(id, count, sizes, reinterpret_cast<void **>(buffers)));
}

void cio::stream::recvv(uint64_t hdc, uint32_t id, uint32_t count, uint64_t *buffers, uint32_t *sizes) noexcept {
    ntos_net_recv(hdc, ntos_net_bind(id, count, sizes, reinterpret_cast<void **>(buffers)));
}
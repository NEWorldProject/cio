#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <vector>
#include <unordered_map>
#include "interfaces/cio.h"
#include "interfaces/temp.h"

static HANDLE hTrap;

// Event Notification API
void cio_file_ev_setup() { hTrap = GetCurrentThread(); }

struct cio_file_completion {
    uint32_t identifier;
    uint32_t error_code;
    uint32_t transferred;
    uint32_t not_used;
};

using cio_completion_list = std::vector<cio_file_completion, temp_alloc<cio_file_completion>>;
using cio_in_flight_table = std::unordered_map<LPOVERLAPPED, uint32_t>;

static cio_completion_list completed; // stores the list of completed external ids
static cio_in_flight_table id_mapping{}; // NOLINT(cert-err58-cpp)

// The overlapped structure must be present at the head for allocation reusing
struct cio_ntos_read_request {
    OVERLAPPED overlapped;
    HANDLE hFile;
    LPVOID lpBuffer;
    DWORD nNumberOfBytesToRead;
    uint32_t identifier;
};

// The overlapped structure must be present at the head for allocation reusing
struct cio_ntos_write_request {
    OVERLAPPED overlapped;
    HANDLE hFile;
    LPCVOID lpBuffer;
    DWORD nNumberOfBytesToWrite;
    uint32_t identifier;
};

// The special construction of the Temp allocation allows the following, though not type safe
static temp_alloc<OVERLAPPED> alOverlapped{}; // FREE ONLY
static temp_alloc<cio_ntos_read_request> alReadRequest{}; // ALLOCATION ONLY
static temp_alloc<cio_ntos_write_request> alWriteRequest{}; // ALLOCATION ONLY

// Trap the current thread until at least one io is completed
uintptr_t cio_file_ev_trap() noexcept {
    {
        cio_completion_list empty{};
        std::swap(completed, empty);
    }
    while (completed.empty()) SleepEx(INFINITE, TRUE);
    return reinterpret_cast<uintptr_t>(completed.data());
}

// flag the completion of a single overlapped io operation
static void cio_ntos_overlapped_completion(
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransferred,
        LPOVERLAPPED lpOverlapped
) noexcept {
    const auto iter = id_mapping.find(lpOverlapped); // it has to be there
    uint32_t identifier = iter->second;
    alOverlapped.deallocate(lpOverlapped, 1);
    id_mapping.erase(iter);
    completed.emplace_back(identifier, dwErrorCode, dwNumberOfBytesTransferred, 0u);
}

static void cio_ntos_apc_start_read(ULONG_PTR parameter) noexcept {
    const auto req = reinterpret_cast<cio_ntos_read_request *>(parameter);
    id_mapping.insert_or_assign(&req->overlapped, req->identifier);
    ReadFileEx(
            req->hFile, req->lpBuffer, req->nNumberOfBytesToRead, &req->overlapped,
            cio_ntos_overlapped_completion
    );
}

void cio_ntos_read(
        uint32_t identifier,
        HANDLE hFile, LPVOID lpBuffer,
        DWORD nNumberOfBytesToRead, uint64_t nOffset
) noexcept {
    const auto req = alReadRequest.allocate(1);
    req->identifier = identifier;
    req->hFile = hFile;
    req->lpBuffer = lpBuffer;
    req->overlapped.Offset = nOffset & 0xFFFFFFFFu;
    req->overlapped.OffsetHigh = (nOffset >> 32ull) & 0xFFFFFFFFu;
    req->overlapped.hEvent = nullptr;
    req->nNumberOfBytesToRead = nNumberOfBytesToRead;
    QueueUserAPC(cio_ntos_apc_start_read, hTrap, reinterpret_cast<ULONG_PTR>(req));
}

static void cio_ntos_apc_start_write(ULONG_PTR parameter) noexcept {
    const auto req = reinterpret_cast<cio_ntos_read_request *>(parameter);
    id_mapping.insert_or_assign(&req->overlapped, req->identifier);
    WriteFileEx(
            req->hFile, req->lpBuffer, req->nNumberOfBytesToRead, &req->overlapped,
            cio_ntos_overlapped_completion
    );
}

void cio_ntos_write(
        uint32_t identifier,
        HANDLE hFile, LPCVOID lpBuffer,
        DWORD nNumberOfBytesToWrite, uint64_t nOffset
) noexcept {
    const auto req = alWriteRequest.allocate(1);
    req->identifier = identifier;
    req->hFile = hFile;
    req->lpBuffer = lpBuffer;
    req->overlapped.Offset = nOffset & 0xFFFFFFFFu;
    req->overlapped.OffsetHigh = (nOffset >> 32ull) & 0xFFFFFFFFu;
    req->overlapped.hEvent = nullptr;
    req->nNumberOfBytesToWrite = nNumberOfBytesToWrite;
    QueueUserAPC(cio_ntos_apc_start_write, hTrap, reinterpret_cast<ULONG_PTR>(req));
}
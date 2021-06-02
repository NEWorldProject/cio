package site.neworld.cio.error

class InconsistentDispatchState: Throwable()

class IOException(): Throwable()

fun ioException(code: Int): IOException {
    return IOException()
}
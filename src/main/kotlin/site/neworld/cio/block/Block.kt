package site.neworld.cio.block

import kotlinx.coroutines.CompletableDeferred
import site.neworld.cio.internal.IODispatch
import site.neworld.cio.unsafe.Span
import site.neworld.utils.AResource
import java.nio.file.Path
import java.util.*

private external fun open(path: ByteArray, flags: Int): Long

private external fun read(dispatch: Int, handle: Long, buffer: Long, offset: Long, length: Long)

private external fun write(dispatch: Int, handle: Long, buffer: Long, offset: Long, length: Long)

private external fun readMulti(
    dispatch: Int, handle: Long,
    bufferHeads: LongArray, bufferSizes: LongArray,
    blockOffsets: LongArray, spanSizes: LongArray
)

private external fun writeMulti(
    dispatch: Int, handle: Long,
    bufferHeads: LongArray, bufferSizes: LongArray,
    blockOffsets: LongArray, spanSizes: LongArray
)

private external fun close(handle: Long)

class Block internal constructor(private val handle: Long) : AResource() {
    suspend fun read(buffer: Span, offset: Long): Int {
        val deferred = CompletableDeferred<Int>()
        val dispatch = IODispatch.assign(deferred)
        read(dispatch, handle, buffer.native, offset, buffer.size)
        return deferred.await()
    }

    suspend fun write(buffer: Span, offset: Long): Int {
        val deferred = CompletableDeferred<Int>()
        val dispatch = IODispatch.assign(deferred)
        write(dispatch, handle, buffer.native, offset, buffer.size)
        return deferred.await()
    }

    private inline fun split(spans: Array<Span>, consumer: (LongArray, LongArray) -> Unit) {
        consumer(
            LongArray(spans.size) { spans[it].native },
            LongArray(spans.size) { spans[it].size }
        )
    }

    suspend fun readMulti(buffers: Array<Span>, overlapped: Array<Span>): Int {
        val deferred = CompletableDeferred<Int>()
        val dispatch = IODispatch.assign(deferred)
        split(buffers) { a, b -> split(overlapped) { c, d -> readMulti(dispatch, handle, a, b, c, d) } }
        return deferred.await()
    }

    suspend fun writeMulti(buffers: Array<Span>, overlapped: Array<Span>): Int {
        val deferred = CompletableDeferred<Int>()
        val dispatch = IODispatch.assign(deferred)
        split(buffers) { a, b -> split(overlapped) { c, d -> writeMulti(dispatch, handle, a, b, c, d) } }
        return deferred.await()
    }

    override fun close() = close(handle)
}

enum class OpenFlag(val v: Int) {
    Read(1), Write(2),
    Create(4), Excl(8), Truncate(16), ExLock(32)
}

fun open(path: Path, vararg flags: OpenFlag): Block {
    var flag = 0
    for (x in flags) flag = flag or x.v
    return Block(open(path.toAbsolutePath().toString().encodeToByteArray(), flag))
}
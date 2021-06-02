package site.neworld.cio.block

import kotlinx.coroutines.CompletableDeferred
import site.neworld.cio.internal.IODispatch
import site.neworld.cio.unsafe.Span

private external fun read(dispatch: Long, handle: Long, buffer: Long, offset: Long, length: Long)

private external fun write(dispatch: Long, handle: Long, buffer: Long, offset: Long, length: Long)

private external fun readMulti(
    dispatch: Long, handle: Long,
    bufferHeads: LongArray, bufferSizes: LongArray,
    blockOffsets: LongArray, spanSizes: LongArray
)

private external fun writeMulti(
    dispatch: Long, handle: Long,
    bufferHeads: LongArray, bufferSizes: LongArray,
    blockOffsets: LongArray, spanSizes: LongArray
)

class Block internal constructor(private val handle: Long) {
    suspend fun read(buffer: Span, offset: Long): Long {
        val deferred = CompletableDeferred<Long>()
        val dispatch = IODispatch.assign(deferred)
        read(dispatch, handle, buffer.native, offset, buffer.size)
        return deferred.await()
    }

    suspend fun write(buffer: Span, offset: Long): Long {
        val deferred = CompletableDeferred<Long>()
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

    suspend fun readMulti(buffers: Array<Span>, overlapped: Array<Span>): Long {
        val deferred = CompletableDeferred<Long>()
        val dispatch = IODispatch.assign(deferred)
        split(buffers) {a, b -> split(overlapped) { c, d -> readMulti(dispatch, handle, a, b, c, d)} }
        return deferred.await()
    }

    suspend fun writeMulti(buffers: Array<Span>, overlapped: Array<Span>): Long {
        val deferred = CompletableDeferred<Long>()
        val dispatch = IODispatch.assign(deferred)
        split(buffers) {a, b -> split(overlapped) { c, d -> writeMulti(dispatch, handle, a, b, c, d)} }
        return deferred.await()
    }
}

package site.neworld.cio.internal

import kotlinx.coroutines.CompletableDeferred
import site.neworld.cio.error.InconsistentDispatchState
import site.neworld.cio.error.ioException
import site.neworld.cio.unsafe.Span
import site.neworld.cio.unsafe.unsafe
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.atomic.AtomicInteger

interface CIONativeDispatch {
    fun batchDispatch(block: Long, count: Long)
}

object IODispatch: CIONativeDispatch {
    private val lookup = ConcurrentHashMap<Int, CompletableDeferred<Int>>()
    private val rotate = AtomicInteger(0)

    tailrec fun assign(obj: CompletableDeferred<Int>): Int {
        val id = rotate.getAndIncrement()
        return if (lookup.putIfAbsent(id, obj) != null) assign(obj) else id
    }

    private fun unblock(id: Int, status: Int, completed: Int) {
        val obj = lookup.remove(id) ?: throw InconsistentDispatchState()
        if (status != 0)
            obj.completeExceptionally(ioException(status))
        else
            obj.complete(completed)
    }

    override fun batchDispatch(block: Long, count: Long) {
        unsafe(Span(block, count * 12)) {
            repeat(count.toInt()) {
                val iBase = it * 12
                val id = getInt(iBase + 0)
                val status = getInt(iBase + 4)
                val completed = getInt(iBase + 8)
                unblock(id, status, completed)
            }
        }
    }
}

fun getDispatch(): CIONativeDispatch = IODispatch

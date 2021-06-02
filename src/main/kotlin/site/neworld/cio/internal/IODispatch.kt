package site.neworld.cio.internal

import kotlinx.coroutines.CompletableDeferred
import site.neworld.cio.error.InconsistentDispatchState
import site.neworld.cio.error.ioException
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.atomic.AtomicLong

object IODispatch {
    private val lookup = ConcurrentHashMap<Long, CompletableDeferred<Long>>()
    private val rotate = AtomicLong(0)

    tailrec fun assign(obj: CompletableDeferred<Long>): Long {
        val id = rotate.getAndIncrement()
        return if (lookup.putIfAbsent(id, obj) != null) assign(obj) else id
    }

    fun unblock(id: Long, status: Int, completed: Long) {
        val obj = lookup.remove(id) ?: throw InconsistentDispatchState()
        if (status != 0)
            obj.completeExceptionally(ioException(status))
        else
            obj.complete(completed)
    }
}
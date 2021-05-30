@file:Suppress("MemberVisibilityCanBePrivate")

package site.neworld.cio.unsafe

import site.neworld.utils.SyncClosable

abstract class Memory(val native: Long, val size: Long) : SyncClosable

class TempMemory(size: Long, native: Long = tempAllocate(size)) : Memory(native, size) {
    private class State(private val native: Long, private val size: Long, obj: Any) : CancelableCleanerState(obj) {
        override fun doRun() = tempFree(native, size)
    }

    private val state = State(native, size, this)

    internal fun drop() = state.drop()
    override fun close() = state.close()
}

class HeapMemory(size: Long, native: Long = heapAllocate(size)) : Memory(native, size) {
    private class State(private val native: Long, private val size: Long, obj: Any) : CancelableCleanerState(obj) {
        override fun doRun() = heapFree(native, size)
    }

    private val state = State(native, size, this)

    internal fun drop() = state.drop()
    override fun close() = state.close()
}

fun relocate(memory: TempMemory, newSize: Long): TempMemory {
    val res = tempRelocate(memory.native, memory.size, newSize)
    if (res != 0L) {
        memory.drop()
        return TempMemory(newSize, res)
    } else throw OutOfMemoryError()
}

fun relocate(memory: HeapMemory, newSize: Long): HeapMemory {
    val res = heapRelocate(memory.native, memory.size, newSize)
    if (res != 0L) {
        memory.drop()
        return HeapMemory(newSize, res)
    } else throw OutOfMemoryError()
}

private external fun tempAllocate(size: Long): Long

private external fun tempFree(mem: Long, size: Long)

private external fun tempRelocate(mem: Long, oldSize: Long, newSize: Long): Long

private external fun heapAllocate(size: Long): Long

private external fun heapFree(mem: Long, size: Long)

private external fun heapRelocate(mem: Long, oldSize: Long, newSize: Long): Long

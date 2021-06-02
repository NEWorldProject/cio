@file:Suppress("MemberVisibilityCanBePrivate")

package site.neworld.cio.unsafe

import site.neworld.utils.SyncClosable

private val LIBRARY = System.loadLibrary("cio")

abstract class Memory(val native: Long, val size: Long) : SyncClosable

class Span(native: Long, size: Long) : Memory(native, size) {
    override fun close() = Unit
}

class TempMemory(size: Long, native: Long = tempAllocate(size)) : Memory(native, size) {
    internal fun drop() = Unit
    override fun close() = tempFree(native, size)
}

class HeapMemory(size: Long, native: Long = heapAllocate(size)) : Memory(native, size) {
    internal fun drop() = Unit
    override fun close() = heapFree(native, size)
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

fun Memory.slice(start: Long, length: Long): Span {
    assert((start > 0) && start < (this.size))
    assert((length > 0) && length < (this.size - start))
    return Span(this.native + start, length)
}

private external fun tempAllocate(size: Long): Long

private external fun tempFree(mem: Long, size: Long)

private external fun tempRelocate(mem: Long, oldSize: Long, newSize: Long): Long

private external fun heapAllocate(size: Long): Long

private external fun heapFree(mem: Long, size: Long)

private external fun heapRelocate(mem: Long, oldSize: Long, newSize: Long): Long

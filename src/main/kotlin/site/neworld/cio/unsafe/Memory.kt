@file:Suppress("MemberVisibilityCanBePrivate")

package site.neworld.cio.unsafe

import site.neworld.utils.SyncClosable

abstract class Memory(val native: Long, val size: Long) : SyncClosable

class TempMemory(native: Long, size: Long) : Memory(native, size) {
    override fun close() {
        TODO("Not yet implemented")
    }
}

class HeapMemory(native: Long, size: Long) : Memory(native, size) {
    override fun close() {
        TODO("Not yet implemented")
    }
}

private external fun tempFree(mem: Long)

private external fun tempAllocate(size: Long): Long

private external fun tempRelocate(mem: Long, oldSize: Long, newSize: Long): Long

private external fun heapFree(mem: Long)

private external fun heapAllocate(size: Long): Long

private external fun heapRelocate(mem: Long, oldSize: Long, newSize: Long): Long

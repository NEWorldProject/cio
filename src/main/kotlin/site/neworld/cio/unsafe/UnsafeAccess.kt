@file:OptIn(ExperimentalUnsignedTypes::class)

package site.neworld.cio.unsafe

import java.lang.Double.doubleToLongBits
import java.lang.Double.longBitsToDouble
import java.lang.Float.floatToIntBits
import java.lang.Float.intBitsToFloat

@JvmInline
value class UnsafeAccess(private val memory: Memory) {
    fun getByteArray(offset: Int, size: Int) = byteArrayFast(size).also {
        assert(offset + size <= memory.size)
        copy(memory.native + offset, size, it)
    }

    fun putByteArray(v: ByteArray, offset: Int) {
        assert(offset + v.size <= memory.size)
        copy(v, memory.native + offset, v.size)
    }

    fun getShortArray(offset: Int, size: Int, le: Boolean) = shortArrayFast(size).also {
        assert(offset + (size shl 1) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putShortArray(v: ShortArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 1) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getIntArray(offset: Int, size: Int, le: Boolean) = intArrayFast(size).also {
        assert(offset + (size shl 2) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putIntArray(v: IntArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 2) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getLongArray(offset: Int, size: Int, le: Boolean) = longArrayFast(size).also {
        assert(offset + (size shl 3) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putLongArray(v: LongArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 3) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getFloatArray(offset: Int, size: Int, le: Boolean) = floatArrayFast(size).also {
        assert(offset + (size shl 2) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putFloatArray(v: FloatArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 2) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getDoubleArray(offset: Int, size: Int, le: Boolean) = doubleArrayFast(size).also {
        assert(offset + (size shl 3) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putDoubleArray(v: DoubleArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 3) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getUByteArray(offset: Int, size: Int) = uByteArrayFast(size).also {
        assert(offset + size <= memory.size)
        copy(memory.native + offset, size, it)
    }

    fun putUByteArray(v: UByteArray, offset: Int) {
        assert(offset + v.size <= memory.size)
        copy(v, memory.native + offset, v.size)
    }

    fun getUShortArray(offset: Int, size: Int, le: Boolean) = uShortArrayFast(size).also {
        assert(offset + (size shl 1) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putUShortArray(v: UShortArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 1) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getUIntArray(offset: Int, size: Int, le: Boolean) = uIntArrayFast(size).also {
        assert(offset + (size shl 2) <= memory.size)
        copy(memory.native + offset, size, it, le)
    }

    fun putUIntArray(v: UIntArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 2) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getULongArray(offset: Int, size: Int, le: Boolean) = uLongArrayFast(size).also {
        assert(offset + (size shl 3) <= size)
        copy(memory.native + offset, size, it, le)
    }

    fun putULongArray(v: ULongArray, offset: Int, le: Boolean) {
        assert(offset + (v.size shl 3) <= memory.size)
        copy(v, memory.native + offset, v.size, le)
    }

    fun getByte(offset: Int): Byte {
        assert(offset < memory.size)
        return UNSAFE.getByte(memory.native + offset)
    }

    fun putByte(offset: Int, v: Byte) {
        assert(offset < memory.size)
        UNSAFE.putByte(memory.native + offset, v)
    }

    fun getShort(offset: Int): Short {
        assert(offset + 1 < memory.size)
        return UNSAFE.getShort(memory.native + offset)
    }

    fun putShort(offset: Int, v: Short) {
        assert(offset + 1 < memory.size)
        UNSAFE.putShort(memory.native + offset, v)
    }

    fun getInt(offset: Int): Int {
        assert(offset + 3 < memory.size)
        return UNSAFE.getInt(memory.native + offset)
    }

    fun putInt(offset: Int, v: Int) {
        assert(offset + 3 < memory.size)
        UNSAFE.putInt(memory.native + offset, v)
    }

    fun getLong(offset: Int): Long {
        assert(offset + 7 < memory.size)
        return UNSAFE.getLong(memory.native + offset)
    }

    fun putLong(offset: Int, v: Long) {
        assert(offset + 7 < memory.size)
        UNSAFE.putLong(memory.native + offset, v)
    }

    fun getFloat(offset: Int): Float {
        assert(offset + 3 < memory.size)
        return UNSAFE.getFloat(memory.native + offset)
    }

    fun putFloat(offset: Int, v: Float) {
        assert(offset + 3 < memory.size)
        UNSAFE.putFloat(memory.native + offset, v)
    }

    fun getDouble(offset: Int): Double {
        assert(offset + 7 < memory.size)
        return UNSAFE.getDouble(memory.native + offset)
    }

    fun putDouble(offset: Int, v: Double) {
        assert(offset + 7 < memory.size)
        UNSAFE.putDouble(memory.native + offset, v)
    }

    fun getShortA(offset: Int): Short {
        assert(offset + 1 < memory.size)
        return UNSAFE.getShortUnaligned(null, memory.native + offset)
    }

    fun putShortA(offset: Int, v: Short) {
        assert(offset + 1 < memory.size)
        UNSAFE.putShortUnaligned(null, memory.native + offset, v)
    }

    fun getIntA(offset: Int): Int {
        assert(offset + 3 < memory.size)
        return UNSAFE.getIntUnaligned(null, memory.native + offset)
    }

    fun putIntA(offset: Int, v: Int) {
        assert(offset + 3 < memory.size)
        UNSAFE.putIntUnaligned(null, memory.native + offset, v)
    }

    fun getLongA(offset: Int): Long {
        assert(offset + 7 < memory.size)
        return UNSAFE.getLongUnaligned(null, memory.native + offset)
    }

    fun putLongA(offset: Int, v: Long) {
        assert(offset + 7 < memory.size)
        UNSAFE.putLongUnaligned(null, memory.native + offset, v)
    }

    fun getFloatA(offset: Int) = intBitsToFloat(getIntA(offset))

    fun putFloatA(offset: Int, v: Float) = putIntA(offset, floatToIntBits(v))

    fun getDoubleA(offset: Int): Double = longBitsToDouble(getLongA(offset))

    fun putDoubleA(offset: Int, v: Double) = putLongA(offset, doubleToLongBits(v))
}
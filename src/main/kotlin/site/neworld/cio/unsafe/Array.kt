@file:OptIn(ExperimentalUnsignedTypes::class)

package site.neworld.cio.unsafe

external fun copy(src: ByteArray, dst: Long, size: Int)

external fun copy(src: Long, size: Int, dst: ByteArray)

external fun copy(src: ShortArray, dst: Long, size: Int, le: Boolean)

external fun copy(src: Long, size: Int, dst: ShortArray, le: Boolean)

external fun copy(src: IntArray, dst: Long, size: Int, le: Boolean)

external fun copy(src: Long, size: Int, dst: IntArray, le: Boolean)

external fun copy(src: LongArray, dst: Long, size: Int, le: Boolean)

external fun copy(src: Long, size: Int, dst: LongArray, le: Boolean)

external fun copy(src: FloatArray, dst: Long, size: Int, le: Boolean)

external fun copy(src: Long, size: Int, dst: FloatArray, le: Boolean)

external fun copy(src: DoubleArray, dst: Long, size: Int, le: Boolean)

external fun copy(src: Long, size: Int, dst: DoubleArray, le: Boolean)

fun copy(src: UByteArray, dst: Long, size: Int) = copy(src.asByteArray(), dst, size)

fun copy(src: Long, size: Int, dst: UByteArray) = copy(src, size, dst.asByteArray())

fun copy(src: UShortArray, dst: Long, size: Int, le: Boolean) = copy(src.asShortArray(), dst, size, le)

fun copy(src: Long, size: Int, dst: UShortArray, le: Boolean) = copy(src, size, dst.asShortArray(), le)

fun copy(src: UIntArray, dst: Long, size: Int, le: Boolean) = copy(src.asIntArray(), dst, size, le)

fun copy(src: Long, size: Int, dst: UIntArray, le: Boolean) = copy(src, size, dst.asIntArray(), le)

fun copy(src: ULongArray, dst: Long, size: Int, le: Boolean) = copy(src.asLongArray(), dst, size, le)

fun copy(src: Long, size: Int, dst: ULongArray, le: Boolean) = copy(src, size, dst.asLongArray(), le)

// NOTE: This is not ideal, but we cannot do it any better without accessing java.internal.misc.Unsafe
fun byteArrayFast(size: Int) = ByteArray(size)

fun shortArrayFast(size: Int) = ShortArray(size)

fun intArrayFast(size: Int) = IntArray(size)

fun longArrayFast(size: Int) = LongArray(size)

fun floatArrayFast(size: Int) = FloatArray(size)

fun doubleArrayFast(size: Int) = DoubleArray(size)

fun uByteArrayFast(size: Int) = ByteArray(size).asUByteArray()

fun uShortArrayFast(size: Int) = ShortArray(size).asUShortArray()

fun uIntArrayFast(size: Int) = IntArray(size).asUIntArray()

fun uLongArrayFast(size: Int) = LongArray(size).asULongArray()

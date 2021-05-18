@file:OptIn(ExperimentalUnsignedTypes::class)
@file:Suppress("UNCHECKED_CAST", "JAVA_MODULE_DOES_NOT_EXPORT_PACKAGE")

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

private inline fun <reified T : Any, R> arrayFast(size: Int) =
    UNSAFE.allocateUninitializedArray(T::class.javaPrimitiveType, size) as R

fun byteArrayFast(size: Int) = arrayFast<Byte, ByteArray>(size)

fun shortArrayFast(size: Int) = arrayFast<Short, ShortArray>(size)

fun intArrayFast(size: Int) = arrayFast<Int, IntArray>(size)

fun longArrayFast(size: Int) = arrayFast<Long, LongArray>(size)

fun floatArrayFast(size: Int) = arrayFast<Float, FloatArray>(size)

fun doubleArrayFast(size: Int) = arrayFast<Double, DoubleArray>(size)

fun uByteArrayFast(size: Int) = arrayFast<Byte, ByteArray>(size).asUByteArray()

fun uShortArrayFast(size: Int) = arrayFast<Short, ShortArray>(size).asUShortArray()

fun uIntArrayFast(size: Int) = arrayFast<Int, IntArray>(size).asUIntArray()

fun uLongArrayFast(size: Int) = arrayFast<Long, LongArray>(size).asULongArray()

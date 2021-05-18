package site.neworld.cio.unsafe

import java.lang.Double.doubleToLongBits
import java.lang.Double.longBitsToDouble
import java.lang.Float.floatToIntBits
import java.lang.Float.intBitsToFloat
import java.lang.Integer.reverseBytes
import java.lang.Long.reverseBytes
import java.lang.Short.reverseBytes

@JvmInline
@OptIn(ExperimentalUnsignedTypes::class)
value class AlignedAccessLE(private val access: UnsafeAccess) {
    fun getByteArray(offset: Int, size: Int) = access.getByteArray(offset, size)
    fun getUByteArray(offset: Int, size: Int) = access.getUByteArray(offset, size)
    fun getShortArray(offset: Int, size: Int) = access.getShortArray(offset, size, true)
    fun getUShortArray(offset: Int, size: Int) = access.getUShortArray(offset, size, true)
    fun getIntArray(offset: Int, size: Int) = access.getIntArray(offset, size, true)
    fun getUIntArray(offset: Int, size: Int) = access.getUIntArray(offset, size, true)
    fun getLongArray(offset: Int, size: Int) = access.getLongArray(offset, size, true)
    fun getULongArray(offset: Int, size: Int) = access.getULongArray(offset, size, true)
    fun getFloatArray(offset: Int, size: Int) = access.getFloatArray(offset, size, true)
    fun getDoubleArray(offset: Int, size: Int) = access.getDoubleArray(offset, size, true)

    fun putByteArray(v: ByteArray, offset: Int) = access.putByteArray(v, offset)
    fun putUByteArray(v: UByteArray, offset: Int) = access.putUByteArray(v, offset)
    fun putShortArray(v: ShortArray, offset: Int) = access.putShortArray(v, offset, true)
    fun putUShortArray(v: UShortArray, offset: Int) = access.putUShortArray(v, offset, true)
    fun putIntArray(v: IntArray, offset: Int) = access.putIntArray(v, offset, true)
    fun putUIntArray(v: UIntArray, offset: Int) = access.putUIntArray(v, offset, true)
    fun putLongArray(v: LongArray, offset: Int) = access.putLongArray(v, offset, true)
    fun putULongArray(v: ULongArray, offset: Int) = access.putULongArray(v, offset, true)
    fun putFloatArray(v: FloatArray, offset: Int) = access.putFloatArray(v, offset, true)
    fun putDoubleArray(v: DoubleArray, offset: Int) = access.putDoubleArray(v, offset, true)

    fun getByte(offset: Int) = access.getByte(offset)
    fun getUByte(offset: Int) = access.getByte(offset).toUByte()
    fun getShort(offset: Int) = reverseBytes(access.getShort(offset))
    fun getUShort(offset: Int) = reverseBytes(access.getShort(offset)).toUShort()
    fun getInt(offset: Int) = reverseBytes(access.getInt(offset))
    fun getUInt(offset: Int) = reverseBytes(access.getInt(offset)).toUInt()
    fun getLong(offset: Int) = reverseBytes(access.getLong(offset))
    fun getULong(offset: Int) = reverseBytes(access.getLong(offset)).toULong()
    fun getFloat(offset: Int) = intBitsToFloat(reverseBytes(access.getInt(offset)))
    fun getDouble(offset: Int) = longBitsToDouble(reverseBytes(access.getLong(offset)))

    fun putByte(offset: Int, v: Byte) = access.putByte(offset, v)
    fun putUByte(offset: Int, v: UByte) = access.putByte(offset, v.toByte())
    fun putShort(offset: Int, v: Short) = access.putShort(offset, reverseBytes(v))
    fun putUShort(offset: Int, v: UShort) = access.putShort(offset, reverseBytes(v.toShort()))
    fun putInt(offset: Int, v: Int) = access.putInt(offset, reverseBytes(v))
    fun putUInt(offset: Int, v: UInt) = access.putInt(offset, reverseBytes(v.toInt()))
    fun putLong(offset: Int, v: Long) = access.putLong(offset, reverseBytes(v))
    fun putULong(offset: Int, v: ULong) = access.putLong(offset, reverseBytes(v.toLong()))
    fun putFloat(offset: Int, v: Float) = access.putInt(offset, reverseBytes(floatToIntBits(v)))
    fun putDouble(offset: Int, v: Double) = access.putLong(offset, reverseBytes(doubleToLongBits(v)))
}
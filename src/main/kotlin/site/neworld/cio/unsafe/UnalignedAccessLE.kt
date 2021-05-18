package site.neworld.cio.unsafe

@JvmInline
@OptIn(ExperimentalUnsignedTypes::class)
value class UnalignedAccessLE(private val access: UnsafeAccess) {
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
    fun getShort(offset: Int) = java.lang.Short.reverseBytes(access.getShortA(offset))
    fun getUShort(offset: Int) = java.lang.Short.reverseBytes(access.getShortA(offset)).toUShort()
    fun getInt(offset: Int) = Integer.reverseBytes(access.getIntA(offset))
    fun getUInt(offset: Int) = Integer.reverseBytes(access.getIntA(offset)).toUInt()
    fun getLong(offset: Int) = java.lang.Long.reverseBytes(access.getLongA(offset))
    fun getULong(offset: Int) = java.lang.Long.reverseBytes(access.getLongA(offset)).toULong()
    fun getFloat(offset: Int) = java.lang.Float.intBitsToFloat(Integer.reverseBytes(access.getIntA(offset)))
    fun getDouble(offset: Int) = java.lang.Double.longBitsToDouble(java.lang.Long.reverseBytes(access.getLongA(offset)))

    fun putByte(offset: Int, v: Byte) = access.putByte(offset, v)
    fun putUByte(offset: Int, v: UByte) = access.putByte(offset, v.toByte())
    fun putShort(offset: Int, v: Short) = access.putShortA(offset, java.lang.Short.reverseBytes(v))
    fun putUShort(offset: Int, v: UShort) = access.putShortA(offset, java.lang.Short.reverseBytes(v.toShort()))
    fun putInt(offset: Int, v: Int) = access.putIntA(offset, Integer.reverseBytes(v))
    fun putUInt(offset: Int, v: UInt) = access.putIntA(offset, Integer.reverseBytes(v.toInt()))
    fun putLong(offset: Int, v: Long) = access.putLongA(offset, java.lang.Long.reverseBytes(v))
    fun putULong(offset: Int, v: ULong) = access.putLongA(offset, java.lang.Long.reverseBytes(v.toLong()))
    fun putFloat(offset: Int, v: Float) = access.putIntA(offset,
        Integer.reverseBytes(java.lang.Float.floatToIntBits(v))
    )
    fun putDouble(offset: Int, v: Double) = access.putLongA(offset,
        java.lang.Long.reverseBytes(java.lang.Double.doubleToLongBits(v))
    )
}
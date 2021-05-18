package site.neworld.cio

fun Memory.getByte(offset: Int): Byte {
    assert(offset < size)
    return UNSAFE.getByte(native + offset)
}

fun Memory.putByte(offset: Int, v: Byte) {
    assert(offset < size)
    UNSAFE.putByte(native + offset, v)
}

fun Memory.getShort(offset: Int): Short {
    assert(offset + 1 < size)
    return UNSAFE.getShortUnaligned(null, native + offset)
}

fun Memory.putShort(offset: Int, v: Short) {
    assert(offset + 1 < size)
    UNSAFE.putShortUnaligned(null, native + offset, v)
}

fun Memory.getInt(offset: Int): Int {
    assert(offset + 3 < size)
    return UNSAFE.getIntUnaligned(null, native + offset)
}

fun Memory.putInt(offset: Int, v: Int) {
    assert(offset + 3 < size)
    UNSAFE.putIntUnaligned(null, native + offset, v)
}

fun Memory.getLong(offset: Int): Long {
    assert(offset + 7 < size)
    return UNSAFE.getLongUnaligned(null, native + offset)
}

fun Memory.putLong(offset: Int, v: Long) {
    assert(offset + 7 < size)
    UNSAFE.putLongUnaligned(null, native + offset, v)
}

fun Memory.getUByte(offset: Int): UByte = getByte(offset).toUByte()

fun Memory.putUByte(offset: Int, v: UByte) = putByte(offset, v.toByte())

fun Memory.getUShort(offset: Int): UShort = getShort(offset).toUShort()

fun Memory.putUShort(offset: Int, v: UShort) = putShort(offset, v.toShort())

fun Memory.getUInt(offset: Int): UInt = getInt(offset).toUInt()

fun Memory.putUInt(offset: Int, v: UInt) = putInt(offset, v.toInt())

fun Memory.getULong(offset: Int): ULong = getLong(offset).toULong()

fun Memory.putULong(offset: Int, v: ULong) = putLong(offset, v.toLong())

package site.neworld.cio

fun Memory.getByteArray(offset: Int, size: Int) = byteArrayFast(size).also {
    assert(offset + size <= this.size)
    copy(native + offset, size, it)
}

fun Memory.putByteArray(v: ByteArray, offset: Int) {
    assert(offset + v.size <= size)
    copy(v, native + offset, v.size)
}

fun Memory.getShortArray(offset: Int, size: Int, le: Boolean) = shortArrayFast(size).also {
    assert(offset + (size shl 1) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putShortArray(v: ShortArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 1) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getIntArray(offset: Int, size: Int, le: Boolean) = intArrayFast(size).also {
    assert(offset + (size shl 2) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putIntArray(v: IntArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 2) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getLongArray(offset: Int, size: Int, le: Boolean) = longArrayFast(size).also {
    assert(offset + (size shl 3) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putLongArray(v: LongArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 3) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getFloatArray(offset: Int, size: Int, le: Boolean) = floatArrayFast(size).also {
    assert(offset + (size shl 2) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putFloatArray(v: FloatArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 2) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getDoubleArray(offset: Int, size: Int, le: Boolean) = doubleArrayFast(size).also {
    assert(offset + (size shl 3) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putDoubleArray(v: DoubleArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 3) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getUByteArray(offset: Int, size: Int) = uByteArrayFast(size).also {
    assert(offset + size <= this.size)
    copy(native + offset, size, it)
}

fun Memory.putUByteArray(v: UByteArray, offset: Int) {
    assert(offset + v.size <= size)
    copy(v, native + offset, v.size)
}

fun Memory.getUShortArray(offset: Int, size: Int, le: Boolean) = uShortArrayFast(size).also {
    assert(offset + (size shl 1) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putUShortArray(v: UShortArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 1) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getUIntArray(offset: Int, size: Int, le: Boolean) = uIntArrayFast(size).also {
    assert(offset + (size shl 2) <= this.size)
    copy(native + offset, size, it, le)
}

fun Memory.putUIntArray(v: UIntArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 2) <= size)
    copy(v, native + offset, v.size, le)
}

fun Memory.getULongArray(offset: Int, size: Int, le: Boolean) = uLongArrayFast(size).also {
    assert(offset + (size shl 3) <= size)
    copy(native + offset, size, it, le)
}

fun Memory.putULongArray(v: ULongArray, offset: Int, le: Boolean) {
    assert(offset + (v.size shl 3) <= size)
    copy(v, native + offset, v.size, le)
}

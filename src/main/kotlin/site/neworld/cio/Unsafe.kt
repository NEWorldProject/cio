package site.neworld.cio

import jdk.internal.misc.Unsafe

internal val UNSAFE = Unsafe::class.java.getDeclaredField("theUnsafe")
    .apply { isAccessible = true }.get(null) as Unsafe

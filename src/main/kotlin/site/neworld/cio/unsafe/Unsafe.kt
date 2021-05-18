@file:OptIn(ExperimentalContracts::class)
package site.neworld.cio.unsafe

import jdk.internal.misc.Unsafe
import site.neworld.utils.safeInit
import site.neworld.utils.use
import kotlin.contracts.ExperimentalContracts
import kotlin.contracts.InvocationKind
import kotlin.contracts.contract

internal val UNSAFE = Unsafe::class.java.getDeclaredField("theUnsafe")
    .apply { isAccessible = true }.get(null) as Unsafe

enum class ReleaseOption {
    NEVER, ALWAYS, ON_FAIL
}

inline fun unsafe(
    memory: Memory,
    block: AlignedAccess.() -> Unit,
    release: ReleaseOption = ReleaseOption.ALWAYS
) {
    contract { callsInPlace(block, InvocationKind.EXACTLY_ONCE) }
    val warp = AlignedAccess(UnsafeAccess(memory))
    when (release) {
        ReleaseOption.NEVER -> warp.block()
        ReleaseOption.ALWAYS -> memory.use { warp.block() }
        ReleaseOption.ON_FAIL -> memory.safeInit { warp.block() }
    }
}

inline fun unsafeLE(
    memory: Memory,
    block: AlignedAccessLE.() -> Unit,
    release: ReleaseOption = ReleaseOption.ALWAYS
) {
    contract { callsInPlace(block, InvocationKind.EXACTLY_ONCE) }
    val warp = AlignedAccessLE(UnsafeAccess(memory))
    when (release) {
        ReleaseOption.NEVER -> warp.block()
        ReleaseOption.ALWAYS -> memory.use { warp.block() }
        ReleaseOption.ON_FAIL -> memory.safeInit { warp.block() }
    }
}

inline fun unsafeUnaligned(
    memory: Memory,
    block: UnalignedAccess.() -> Unit,
    release: ReleaseOption = ReleaseOption.ALWAYS
) {
    contract { callsInPlace(block, InvocationKind.EXACTLY_ONCE) }
    val warp = UnalignedAccess(UnsafeAccess(memory))
    when (release) {
        ReleaseOption.NEVER -> warp.block()
        ReleaseOption.ALWAYS -> memory.use { warp.block() }
        ReleaseOption.ON_FAIL -> memory.safeInit { warp.block() }
    }
}

inline fun unsafeUnalignedLE(
    memory: Memory,
    block: UnalignedAccessLE.() -> Unit,
    release: ReleaseOption = ReleaseOption.ALWAYS
) {
    contract { callsInPlace(block, InvocationKind.EXACTLY_ONCE) }
    val warp = UnalignedAccessLE(UnsafeAccess(memory))
    when (release) {
        ReleaseOption.NEVER -> warp.block()
        ReleaseOption.ALWAYS -> memory.use { warp.block() }
        ReleaseOption.ON_FAIL -> memory.safeInit { warp.block() }
    }
}
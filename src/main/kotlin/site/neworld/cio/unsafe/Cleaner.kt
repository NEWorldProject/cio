package site.neworld.cio.unsafe

import java.lang.ref.Cleaner
import java.util.concurrent.atomic.AtomicBoolean

internal val cleaner = Cleaner.create()

@Suppress("LeakingThis")
internal abstract class CancelableCleanerState(obj: Any) : Runnable {
    private val cancel = AtomicBoolean(false)

    // The leak of 'this' is intentional to reduce object count, so use the class carefully
    private val cleanable = cleaner.register(obj, this)

    // the delegation
    final override fun run() = if (!cancel.get()) doRun() else Unit
    protected abstract fun doRun()

    // interface functions
    fun close() = cleanable.clean()
    fun drop() = cancel.set(true).also { close() }
}
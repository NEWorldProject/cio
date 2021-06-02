package site.neworld.cio.unsafe;

import jdk.incubator.foreign.*;

import java.lang.invoke.VarHandle;
import java.nio.ByteOrder;

public class Foreign {
    private static final VarHandle byte_s = MemoryHandles.varHandle(byte.class, ByteOrder.nativeOrder());
    private static final VarHandle char_BE = handle(MemoryLayouts.BITS_16_BE, char.class);
    private static final VarHandle short_BE = handle(MemoryLayouts.BITS_16_BE, short.class);
    private static final VarHandle int_BE = handle(MemoryLayouts.BITS_32_BE, int.class);
    private static final VarHandle float_BE = handle(MemoryLayouts.BITS_32_BE, float.class);
    private static final VarHandle long_BE = handle(MemoryLayouts.BITS_64_BE, long.class);
    private static final VarHandle double_BE = handle(MemoryLayouts.BITS_64_BE, double.class);
    private static final VarHandle char_BEA = unalignedHandle(MemoryLayouts.BITS_16_BE, char.class);
    private static final VarHandle short_BEA = unalignedHandle(MemoryLayouts.BITS_16_BE, short.class);
    private static final VarHandle int_BEA = unalignedHandle(MemoryLayouts.BITS_32_BE, int.class);
    private static final VarHandle float_BEA = unalignedHandle(MemoryLayouts.BITS_32_BE, float.class);
    private static final VarHandle long_BEA = unalignedHandle(MemoryLayouts.BITS_64_BE, long.class);
    private static final VarHandle double_BEA = unalignedHandle(MemoryLayouts.BITS_64_BE, double.class);
    private static final MemorySegment global = MemoryAddress.ofLong(0).asSegmentRestricted(0x7FFFFFFFFFFFFFFFL).share();

    private static VarHandle handle(ValueLayout elementLayout, Class<?> carrier) {
        return MemoryHandles.varHandle(carrier, elementLayout.order());
    }

    private static VarHandle unalignedHandle(ValueLayout elementLayout, Class<?> carrier) {
        return MemoryHandles.varHandle(carrier, 1, elementLayout.order());
    }

    public static byte getByte(long address) {
        return (byte) byte_s.get(global, address);
    }

    public static void putByte(long address, byte v) {
        byte_s.set(global, address, v);
    }

    public static short getShort(long address) {
        return (short) short_BE.get(global, address);
    }

    public static void putShort(long address, short v) {
        short_BE.set(global, address, v);
    }

    public static int getInt(long address) {
        return (int) int_BE.get(global, address);
    }

    public static void putInt(long address, int v) {
        int_BE.set(global, address, v);
    }

    public static long getLong(long address) {
        return (long) long_BE.get(global, address);
    }

    public static void putLong(long address, long v) {
        long_BE.set(global, address, v);
    }

    public static float getFloat(long address) {
        return (float) float_BE.get(global, address);
    }

    public static void putFloat(long address, float v) {
        float_BE.set(global, address, v);
    }

    public static double getDouble(long address) {
        return (double) double_BE.get(global, address);
    }

    public static void putDouble(long address, double v) {
        double_BE.set(global, address, v);
    }

    public static short getShortA(long address) {
        return (short) short_BEA.get(global, address);
    }

    public static void putShortA(long address, short v) {
        short_BEA.set(global, address, v);
    }

    public static int getIntA(long address) {
        return (int) int_BEA.get(global, address);
    }

    public static void putIntA(long address, int v) {
        int_BEA.set(global, address, v);
    }

    public static long getLongA(long address) {
        return (long) long_BEA.get(global, address);
    }

    public static void putLongA(long address, long v) {
        long_BEA.set(global, address, v);
    }

    public static float getFloatA(long address) {
        return (float) float_BEA.get(global, address);
    }

    public static void putFloatA(long address, float v) {
        float_BEA.set(global, address, v);
    }

    public static double getDoubleA(long address) {
        return (double) double_BEA.get(global, address);
    }

    public static void putDoubleA(long address, double v) {
        double_BEA.set(global, address, v);
    }
}

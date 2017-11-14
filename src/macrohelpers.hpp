/*
 * Copyright (C) 2017 Larry Lopez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef MACROHELPERS_HPP
#define MACROHELPERS_HPP

#include <cstdint>
#include <QtGlobal>

/// define endianess and prefetching
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
    #define __LITTLE_ENDIAN 1234
    #define __BIG_ENDIAN    4321
    #define __PDP_ENDIAN	3412
    #define __BYTE_ORDER    __LITTLE_ENDIAN

    /// includes intrinsic cpu instructions.
    #include <intrin.h>
    #if defined(__MINGW32__) || defined(__MINGW64__)
        // includes intrinsic cpu instructions.
        #include <x86intrin.h>
        #define PREFETCH(location) __builtin_prefetch(location)
    #else
        #define PREFETCH(location) _mm_prefetch(location, _MM_HINT_T0)
    #endif
#else
    // defines __BYTE_ORDER as __LITTLE_ENDIAN or __BIG_ENDIAN
    #include <sys/param.h>
    // includes intrinsic cpu instructions.
    #include <x86intrin.h>
    #ifdef __GNUC__
        #define PREFETCH(location) __builtin_prefetch(location)
    #else
        #define PREFETCH(location) ;
    #endif
#endif

/// Try to determine endianness at runtime.
#define IS_BIG_ENDIANV1 (!*(unsigned char *)&(uint16_t){1})
#define IS_BIG_ENDIANV2 (*(uint16_t *)"\0\xff" < 0x100)
#define IS_BIG_ENDIANV3 (!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)

#define QINT16CAST(x)   (static_cast<qint16>(x))
#define QUINT16CAST(x)  (static_cast<quint16>(x))
#define QINT32CAST(x)   (static_cast<qint32>(x))
#define QUINT32CAST(x)  (static_cast<quint32>(x))
#define QINT64CAST(x)   (static_cast<qint64>(x))
#define QUINT64CAST(x)  (static_cast<quint64>(x))

/// Cast and decorate integer constants if needed.
#define QINT16CONST(x)  (QINT16CAST(INT16_C(x)))
#define QUINT16CONST(x) (QUINT16CAST(UINT16_C(x)))
#define QINT32CONST(x)  (QINT32CAST(INT32_C(x)))
#define QUINT32CONST(x) (QUINT32CAST(UINT32_C(x)))
#define QINT64CONST(x)  (QINT64CAST(INT64_C((x))
#define QUINT64CONST(x) (QUINT64CAST(UINT64_C(x)))

#ifndef GET16BITS
    #define GET16BITS(x) (*reinterpret_cast<const quint16*>(x))
#endif

#ifndef GET32BITS
    #define GET32BITS(x) (*reinterpret_cast<const quint32*>(x))
#endif

#ifndef GET64BITS
    #define GET64BITS(x) (*reinterpret_cast<const quint64*>(x))
#endif

#endif // MACROHELPERS_HPP

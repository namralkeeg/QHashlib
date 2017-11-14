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
#include <noncryptographic/fnv1hash.hpp>

namespace QHashlib { namespace noncryptographic {

Fnv1Hash::Fnv1Hash(const FnvBits &bits) : FnvBase(bits)
{ }

void Fnv1Hash::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    switch (m_bits) {
    case FnvBits::Bits32:
    {
        quint32 hash32 = m_hash.toUInt();
        calcFnv1Hash<quint32>(data, dataLength, startIndex, fnvPrime32, hash32);
        m_hash.setValue(hash32);
    }
        break;
    case FnvBits::Bits64:
    {
        quint64 hash64 = m_hash.toULongLong();
        calcFnv1Hash<quint64>(data, dataLength, startIndex, fnvPrime64, hash64);
        m_hash.setValue(hash64);
    }
        break;
    default:
        break;
    }
}

} // namespace noncryptographic
} // namespace QHashlib

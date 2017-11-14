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
#ifndef FNV1HASH_HPP
#define FNV1HASH_HPP

#include <noncryptographic/fnvbase.hpp>

namespace QHashlib { namespace noncryptographic {

class Fnv1Hash : public FnvBase
{
public:
    Fnv1Hash(const FnvBits &bits = FnvBits::Bits32);

protected:
    virtual void hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex) override;

    template<typename T>
    void calcFnv1Hash(const void* data, quint64 dataLength, const quint64 &startIndex,
                      T fnvPrime, T& hashValue);
};

template<typename T>
void Fnv1Hash::calcFnv1Hash(const void *data, quint64 dataLength, const quint64 &startIndex, T fnvPrime, T &hashValue)
{
    const quint8 *current = static_cast<const quint8*>(data) + startIndex;

    for (quint64 i = 0; i < dataLength; ++current, ++i)
    {
        hashValue = (fnvPrime * hashValue) ^ *current;
    }
}

} // namespace noncryptographic
} // namespace QHashlib

#endif // FNV1HASH_HPP

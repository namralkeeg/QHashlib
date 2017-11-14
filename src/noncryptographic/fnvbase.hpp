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
#ifndef FNV_HPP
#define FNV_HPP

#include <enums.hpp>
#include <hashalgorithm.hpp>
#include <QVariant>

namespace QHashlib { namespace noncryptographic {

enum class FnvBits : quint16
{
    Bits32  =  32,
    Bits64  =  64,
};

class FnvBase : public HashAlgorithm
{
public:
    virtual QByteArray hashFinal() override;
    virtual quint32 hashSize() override;
    virtual void initialize() override;
    virtual ~FnvBase() { }
protected:
//    template<typename T>
//    void calcFnv1Hash(const void* data, quint64 dataLength, const quint64 &startIndex,
//                      T fnvPrime, T& hashValue);
//    template<typename T>
//    void calcFnv1aHash(const void* data, quint64 dataLength, const quint64 &startIndex,
//                       T fnvPrime, T& hashValue);
    FnvBase(const FnvBits &bits = FnvBits::Bits32);

    static const quint32 fnvPrime32 = UINT32_C(16777619);
    static const quint32 offsetBasis32 = UINT32_C(2166136261);
    static const quint64 fnvPrime64 = UINT64_C(1099511628211);
    static const quint64 offsetBasis64 = UINT64_C(14695981039346656037);

    FnvBits m_bits;
    QVariant m_hash;
};

//quint32 FnvBase::hashSize()
//{
//    return static_cast<quint32>(enumToIntegral(m_bits));
//}

//void FnvBase::initialize()
//{
//    m_hashValue.clear();

//    switch (m_bits) {
//    case FnvBits::Bits32:
//        m_hash.setValue(offsetBasis32);
//        break;
//    case FnvBits::Bits64:
//        m_hash.setValue(offsetBasis64);
//        break;
//    default:
//        break;
//    }
//}

//FnvBase::FnvBase(const FnvBits &bits) : m_bits(bits)
//{
//    initialize();
//}

//template<typename T>
//void FnvBase::calcFnv1Hash(const void *data, quint64 dataLength, const quint64 &startIndex,
//                           T fnvPrime,
//                           T &hashValue)
//{
//    const quint8 *current = static_cast<const quint8*>(data) + startIndex;

//    for (quint64 i = 0; i < dataLength; ++current, ++i)
//    {
//        hashValue = (fnvPrime * hashValue) ^ *current;
//    }
//}

//template<typename T>
//void FnvBase::calcFnv1aHash(const void *data, quint64 dataLength, const quint64 &startIndex,
//                            T fnvPrime,
//                            T &hashValue)
//{
//    const quint8 *current = static_cast<const quint8*>(data) + startIndex;

//    for (quint64 i = 0; i < dataLength; ++current, ++i)
//    {
//        hashValue = (*current ^ hashValue) * fnvPrime;
//    }
//}

} // namespace noncryptographic
} // namespace QHashlib

#endif // FNV_HPP

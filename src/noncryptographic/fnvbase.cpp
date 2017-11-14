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
#include <noncryptographic/fnvbase.hpp>
#include <QDataStream>

namespace QHashlib { namespace noncryptographic {

QByteArray FnvBase::hashFinal()
{
//    const quint64 byteSize = hashSize() / std::numeric_limits<quint8>::digits;
    QByteArray qba;
//    qba.reserve(byteSize);
    QDataStream stream(&qba, QIODevice::WriteOnly);
    // Make sure the data is Big-Endian
    stream.setByteOrder(QDataStream::BigEndian);

    switch (m_bits) {
    case FnvBits::Bits32:
//    {
//        quint32 data = native_to_big(m_hash.toUInt());
//        qba = QByteArray(reinterpret_cast<const char*>(&data), sizeof(quint32));
        stream << static_cast<quint32>(m_hash.toUInt());
//    }
        break;
    case FnvBits::Bits64:
//    {
//        quint64 data = native_to_big(m_hash.toULongLong());
//        qba = QByteArray(reinterpret_cast<const char*>(&data), sizeof(quint64));
        stream << static_cast<quint64>(m_hash.toULongLong());
//    }
        break;
    default:
        break;
    }

    return qba;
}

quint32 FnvBase::hashSize()
{
    return static_cast<quint32>(enumToIntegral(m_bits));
}

void FnvBase::initialize()
{
    m_hashValue.clear();

    switch (m_bits) {
    case FnvBits::Bits32:
        m_hash.setValue(offsetBasis32);
        break;
    case FnvBits::Bits64:
        m_hash.setValue(offsetBasis64);
        break;
    default:
        break;
    }
}

FnvBase::FnvBase(const FnvBits &bits) : m_bits(bits)
{
    initialize();
}


} // namespace noncryptographic
} // namespace QHashlib

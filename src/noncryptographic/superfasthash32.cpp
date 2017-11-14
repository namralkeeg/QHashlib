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
#include <noncryptographic/superfasthash32.hpp>
#include <macrohelpers.hpp>
#include <QDataStream>

namespace QHashlib { namespace noncryptographic {

SuperFastHash32::SuperFastHash32()
{
    initialize();
}

QByteArray SuperFastHash32::hashFinal()
{
    // Force "avalanching" of final 127 bits
    m_hash ^= m_hash << 3;
    m_hash += m_hash >> 5;
    m_hash ^= m_hash << 4;
    m_hash += m_hash >> 17;
    m_hash ^= m_hash << 25;
    m_hash += m_hash >> 6;

    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    // Make sure the data is Big-Endian
    stream.setByteOrder(QDataStream::BigEndian);
    stream << m_hash;

    return qba;
}

quint32 SuperFastHash32::hashSize()
{
    return m_hashSize;
}

void SuperFastHash32::initialize()
{
    m_hash = 0;
    m_hashValue.clear();
}

void SuperFastHash32::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    const quint8 *current = static_cast<const quint8*>(data) + startIndex;
    quint64 length = dataLength;
    quint32 temp;
    qint32 rem = length & 3;

    if (m_hash == 0)
        m_hash = static_cast<quint32>(length);

    length >>= 2;

    for (; length > 0; length--)
    {
        m_hash  += GET16BITS(current);
        temp    = (GET16BITS(current+2) << 11) ^ m_hash;
        m_hash  = (m_hash << 16) ^ temp;
        current += 2 * sizeof(quint32);
        m_hash  += m_hash >> 11;
    }

    // Handle end cases
    switch (rem)
    {
        case 3: m_hash += GET16BITS(current);
                m_hash ^= m_hash << 16;
                m_hash ^= static_cast<qint8>(current[sizeof(quint16)]) << 18;
                m_hash += m_hash >> 11;
                break;
        case 2: m_hash += GET16BITS(current);
                m_hash ^= m_hash << 11;
                m_hash += m_hash >> 17;
                break;
        case 1: m_hash += *reinterpret_cast<const qint8*>(current);
                m_hash ^= m_hash << 10;
                m_hash += m_hash >> 1;
    }
}

} // namespace noncryptographic
} // namespace QHashlib

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
#include <noncryptographic/djb2hash32.hpp>
#include <QDataStream>

namespace QHashlib { namespace noncryptographic {

Djb2Hash32::Djb2Hash32(const quint32 &defaultSeed) : m_defaultSeed(defaultSeed)
{
    initialize();
}

QByteArray Djb2Hash32::hashFinal()
{
    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    // Make sure the data is Big-Endian
    stream.setByteOrder(QDataStream::BigEndian);
    stream << m_hash;

    return qba;
}

quint32 Djb2Hash32::hashSize()
{
    return m_hashSize;
}

void Djb2Hash32::initialize()
{
    m_hash = m_defaultSeed;
    m_hashValue.clear();
}

void Djb2Hash32::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    const quint8 *current = static_cast<const quint8*>(data) + startIndex;

    for (quint64 i = 0; i < dataLength; ++current, ++i)
    {
        m_hash = ((m_hash << 5) + m_hash) + *current; /* hash * 33 + c */
    }
}

} // namespace noncryptographic
} // namespace QHashlib

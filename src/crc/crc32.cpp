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
#include <crc/crc32.hpp>
#include <endian.hpp>
#include <QDataStream>

namespace QHashlib { namespace crc {

Crc32::Crc32(const uint32_t &polynomial, const uint32_t &seed) :
    HashAlgorithm(), m_polynomial(polynomial), m_seed(seed)
{
    initialize();
    initializeTable();
}

QByteArray Crc32::hashFinal()
{
    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    // Make sure the data is Big-Endian
    stream.setByteOrder(QDataStream::BigEndian);

#if defined(Q_BYTE_ORDER) && (Q_BYTE_ORDER != 0) && (Q_BYTE_ORDER == Q_BIG_ENDIAN)
    stream << QHashlib::swap(m_hash);
#else
    stream << m_hash;
#endif

    return qba;
}

quint32 Crc32::hashSize()
{
    return m_hashSize;
}

void Crc32::initialize()
{
    m_hash = m_seed;
    m_hashValue.clear();
}

void Crc32::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    quint32 crc = ~m_hash; // same as previousCrc32 ^ 0xFFFFFFFF
    const quint8* currentByte = static_cast<const quint8*>(data) + startIndex;
    const quint32* current = reinterpret_cast<const quint32*>(currentByte);
    quint64 numBytes = dataLength;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const quint64 Unroll = 4;
    const quint64 BytesAtOnce = 16 * Unroll;

    // Process 64 bytes each pass.
    while (numBytes >= BytesAtOnce)
    {
      for (quint64 unrolling = 0; unrolling < Unroll; unrolling++)
      {
  #if Q_BYTE_ORDER == Q_BIG_ENDIAN
          quint32 one   = *current++ ^ swap(crc);
          quint32 two   = *current++;
          quint32 three = *current++;
          quint32 four  = *current++;
          crc  = m_lookupTable[ 0][ four         & 0xFF] ^
                  m_lookupTable[ 1][(four  >>  8) & 0xFF] ^
                  m_lookupTable[ 2][(four  >> 16) & 0xFF] ^
                  m_lookupTable[ 3][(four  >> 24) & 0xFF] ^
                  m_lookupTable[ 4][ three        & 0xFF] ^
                  m_lookupTable[ 5][(three >>  8) & 0xFF] ^
                  m_lookupTable[ 6][(three >> 16) & 0xFF] ^
                  m_lookupTable[ 7][(three >> 24) & 0xFF] ^
                  m_lookupTable[ 8][ two          & 0xFF] ^
                  m_lookupTable[ 9][(two   >>  8) & 0xFF] ^
                  m_lookupTable[10][(two   >> 16) & 0xFF] ^
                  m_lookupTable[11][(two   >> 24) & 0xFF] ^
                  m_lookupTable[12][ one          & 0xFF] ^
                  m_lookupTable[13][(one   >>  8) & 0xFF] ^
                  m_lookupTable[14][(one   >> 16) & 0xFF] ^
                  m_lookupTable[15][(one   >> 24) & 0xFF];
#else
          quint32 one   = *current++ ^ crc;
          quint32 two   = *current++;
          quint32 three = *current++;
          quint32 four  = *current++;
          crc  = m_lookupTable[ 0][(four  >> 24) & 0xFF] ^
                  m_lookupTable[ 1][(four  >> 16) & 0xFF] ^
                  m_lookupTable[ 2][(four  >>  8) & 0xFF] ^
                  m_lookupTable[ 3][ four         & 0xFF] ^
                  m_lookupTable[ 4][(three >> 24) & 0xFF] ^
                  m_lookupTable[ 5][(three >> 16) & 0xFF] ^
                  m_lookupTable[ 6][(three >>  8) & 0xFF] ^
                  m_lookupTable[ 7][ three        & 0xFF] ^
                  m_lookupTable[ 8][(two   >> 24) & 0xFF] ^
                  m_lookupTable[ 9][(two   >> 16) & 0xFF] ^
                  m_lookupTable[10][(two   >>  8) & 0xFF] ^
                  m_lookupTable[11][ two          & 0xFF] ^
                  m_lookupTable[12][(one   >> 24) & 0xFF] ^
                  m_lookupTable[13][(one   >> 16) & 0xFF] ^
                  m_lookupTable[14][(one   >>  8) & 0xFF] ^
                  m_lookupTable[15][ one          & 0xFF];
#endif
      }

      numBytes -= BytesAtOnce;
      currentByte += BytesAtOnce;
    }

    // remaining 1 to 63 bytes (standard algorithm)
    while (numBytes-- != 0)
        crc = (crc >> 8) ^ m_lookupTable[0][(crc & 0xFF) ^ *currentByte++];

    m_hash = ~crc;
}

void Crc32::initializeTable()
{
    quint32 entry;
    for (quint32 i = 0; i < TableEntries; ++i)
    {
        entry = i;
        for (auto j = 0; j < 8; ++j)
        {
            entry = (entry >> 1) ^ ((entry & 1) * m_polynomial);
        }

        m_lookupTable[0][i] = entry;
    }

    for (quint32 i = 0; i < TableEntries; ++i)
    {
        for (quint32 slice = 1; slice < MaxSlice; ++slice)
        {
            m_lookupTable[slice][i] =
                    (m_lookupTable[slice - 1][i] >> 8) ^ m_lookupTable[0][m_lookupTable[slice - 1][i] & 0xFF];
        }
    }
}

} // namesapce crc
} // namespace QHashlib

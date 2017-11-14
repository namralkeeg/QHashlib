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
#include <noncryptographic/xxhash32.hpp>
#include <endian.hpp>
#include <algorithm>
#include <QDataStream>

namespace QHashlib { namespace noncryptographic {

#ifndef ROTATELEFT_M
    #define ROTATELEFT_M(x,y) QHashlib::rotateLeft((x),(y))
#endif

XxHash32::XxHash32(const quint32 &seed) : m_seed(seed)
{
    initialize();
}

QByteArray XxHash32::hashFinal()
{
    quint32 result = static_cast<quint32>(m_totalLength);

     // fold 128 bit state into one single 32 bit value
     if (m_totalLength >= MaxBufferSize)
     {
         result += ROTATELEFT_M(m_state[0],  1) +
                   ROTATELEFT_M(m_state[1],  7) +
                   ROTATELEFT_M(m_state[2], 12) +
                   ROTATELEFT_M(m_state[3], 18);
     }
     else
     {
         // internal state wasn't set in add(), therefore original seed is still stored in state2
         result += m_state[2] + Prime5;
     }

     // process remaining bytes in temporary buffer
     const quint8* data = m_buffer.data();
     // point beyond last byte
     const quint8* stop = data + m_bufferSize;

     // at least 4 bytes left ? => eat 4 bytes per step
     for (; data + 4 <= stop; data += 4)
         result = ROTATELEFT_M(result + GET32BITS(data) * Prime3, 17) * Prime4;

     // take care of remaining 0..3 bytes, eat 1 byte per step
     while (data != stop)
         result = ROTATELEFT_M(result + (*data++) * Prime5, 11) * Prime1;

     // mix bits
     result ^= result >> 15;
     result *= Prime2;
     result ^= result >> 13;
     result *= Prime3;
     result ^= result >> 16;

     QByteArray qba;
     QDataStream stream(&qba, QIODevice::WriteOnly);

     // Make sure the data is Big-Endian
     stream.setByteOrder(QDataStream::BigEndian);
     stream << result;

     return qba;
}

quint32 XxHash32::hashSize()
{
    return m_hashSize;
}

void XxHash32::initialize()
{
    m_state[0] = m_seed + Prime1 + Prime2;
    m_state[1] = m_seed + Prime2;
    m_state[2] = m_seed;
    m_state[3] = m_seed - Prime1;
    m_bufferSize  = 0;
    m_totalLength = 0;
    m_hashValue.clear();
    std::fill(std::begin(m_buffer), std::end(m_buffer), 0);
}

void XxHash32::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    // byte-wise access
    const quint8* current = static_cast<const quint8*>(data) + startIndex;

    quint64 length = dataLength;
    m_totalLength += length;
    //const unsigned char* data = (const unsigned char*)input;

    // unprocessed old data plus new data still fit in temporary buffer ?
    if (m_bufferSize + length < MaxBufferSize)
    {
        // just add new data
        while (length-- > 0)
            m_buffer[m_bufferSize++] = *current++;
    }
    else
    {
        // point beyond last byte
        const quint8* stop      = current + length;
        const quint8* stopBlock = stop - MaxBufferSize;

        // some data left from previous update ?
        if (m_bufferSize > 0)
        {
            // make sure temporary buffer is full (16 bytes)
            while (m_bufferSize < MaxBufferSize)
                m_buffer[m_bufferSize++] = *current++;

            // process these 16 bytes (4x4)
            process(m_buffer.data(), m_state[0], m_state[1], m_state[2], m_state[3]);
        }

        // copying state to local variables helps optimizer A LOT
        quint32 s0 = m_state[0], s1 = m_state[1], s2 = m_state[2], s3 = m_state[3];

        // 16 bytes at once
        while (current <= stopBlock)
        {
            // local variables s0..s3 instead of state[0]..state[3] are much faster
            process(current, s0, s1, s2, s3);
            current += 16;
        }

        // copy back
        m_state[0] = s0; m_state[1] = s1; m_state[2] = s2; m_state[3] = s3;

        // copy remainder to temporary buffer
        m_bufferSize = static_cast<quint32>(stop - current);
        std::copy(current, current + m_bufferSize, std::begin(m_buffer));
    }
}

void XxHash32::process(const void *data, quint32 &state0, quint32 &state1, quint32 &state2, quint32 &state3)
{
    const quint32* block = static_cast<const quint32*>(data);

    state0 = ROTATELEFT_M(state0 + block[0] * Prime2, 13) * Prime1;
    state1 = ROTATELEFT_M(state1 + block[1] * Prime2, 13) * Prime1;
    state2 = ROTATELEFT_M(state2 + block[2] * Prime2, 13) * Prime1;
    state3 = ROTATELEFT_M(state3 + block[3] * Prime2, 13) * Prime1;
}

} // namespace noncryptographic
} // namespace QHashlib

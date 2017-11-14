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
#include <noncryptographic/xxhash64.hpp>
#include <endian.hpp>
#include <algorithm>
#include <QDataStream>

namespace QHashlib { namespace noncryptographic {

#ifndef ROTATELEFT_M
    #define ROTATELEFT_M(x,y) QHashlib::rotateLeft((x),(y))
#endif

XxHash64::XxHash64(const quint64 &seed) : m_seed(seed)
{
    initialize();
}

QByteArray XxHash64::hashFinal()
{
    // fold 256 bit state into one single 64 bit value
    quint64 result;
    if (m_totalLength >= MaxBufferSize)
    {
        result = ROTATELEFT_M(m_state[0],  1) +
                 ROTATELEFT_M(m_state[1],  7) +
                 ROTATELEFT_M(m_state[2], 12) +
                 ROTATELEFT_M(m_state[3], 18);
        result = (result ^ processSingle(0, m_state[0])) * Prime1 + Prime4;
        result = (result ^ processSingle(0, m_state[1])) * Prime1 + Prime4;
        result = (result ^ processSingle(0, m_state[2])) * Prime1 + Prime4;
        result = (result ^ processSingle(0, m_state[3])) * Prime1 + Prime4;
    }
    else
    {
        // internal state wasn't set in add(), therefore original seed is still stored in state2
        result = m_state[2] + Prime5;
    }

    result += m_totalLength;

    // process remaining bytes in temporary buffer
    const quint8* data = m_buffer.data();
    // point beyond last byte
    const quint8* stop = data + m_bufferSize;

    // at least 8 bytes left ? => eat 8 bytes per step
    for (; data + 8 <= stop; data += 8)
        result = ROTATELEFT_M(result ^ processSingle(0, GET64BITS(data)), 27) * Prime1 + Prime4;

    // 4 bytes left ? => eat those
    if (data + 4 <= stop)
    {
        result = ROTATELEFT_M(result ^ GET32BITS(data) * Prime1, 23) * Prime2 + Prime3;
        data  += 4;
    }

    // take care of remaining 0..3 bytes, eat 1 byte per step
    while (data != stop)
        result = ROTATELEFT_M(result ^ (*data++) * Prime5, 11) * Prime1;

    // mix bits
    result ^= result >> 33;
    result *= Prime2;
    result ^= result >> 29;
    result *= Prime3;
    result ^= result >> 32;

    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    // Make sure the data is Big-Endian
    stream.setByteOrder(QDataStream::BigEndian);
    stream << result;

    return qba;
}

quint32 XxHash64::hashSize()
{
    return m_hashSize;
}

void XxHash64::initialize()
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

void XxHash64::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    quint64 length = dataLength;
    m_totalLength += length;

    // byte-wise access
    const quint8* current = static_cast<const quint8*>(data) + startIndex;

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

            // process these 32 bytes (4x8)
            process(m_buffer.data(), m_state[0], m_state[1], m_state[2], m_state[3]);
        }

        // copying state to local variables helps optimizer A LOT
        quint64 s0 = m_state[0], s1 = m_state[1], s2 = m_state[2], s3 = m_state[3];
        // 32 bytes at once
        while (current <= stopBlock)
        {
            // local variables s0..s3 instead of state[0]..state[3] are much faster
            process(current, s0, s1, s2, s3);
            current += 32;
        }
        // copy back
        m_state[0] = s0; m_state[1] = s1; m_state[2] = s2; m_state[3] = s3;

        // copy remainder to temporary buffer
        m_bufferSize = static_cast<quint32>(stop - current);
        std::copy(current, current + m_bufferSize, std::begin(m_buffer));
    }
}

quint64 XxHash64::processSingle(const quint64 &previous, const quint64 &input)
{
    return ROTATELEFT_M(previous + input * Prime2, 31) * Prime1;
}

void XxHash64::process(const void *data, quint64 &state0, quint64 &state1, quint64 &state2, quint64 &state3)
{
    const quint64* block = static_cast<const quint64*>(data);
    state0 = processSingle(state0, block[0]);
    state1 = processSingle(state1, block[1]);
    state2 = processSingle(state2, block[2]);
    state3 = processSingle(state3, block[3]);
}

} // namespace noncryptographic
} // namespace QHashlib

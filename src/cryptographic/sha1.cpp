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
#include <cryptographic/sha1.hpp>
#include <endian.hpp>
#include <algorithm>
//#include <iterator>

namespace QHashlib { namespace cryptographic {

namespace {

#ifndef ROTATELEFT_M
    #define ROTATELEFT_M(x,y) QHashlib::rotateLeft((x),(y))
#endif

// mix functions for processBlock()
inline uint32_t f1(uint32_t b, uint32_t c, uint32_t d)
{
    return d ^ (b & (c ^ d)); // original: f = (b & c) | ((~b) & d);
}

inline uint32_t f2(uint32_t b, uint32_t c, uint32_t d)
{
    return b ^ c ^ d;
}

inline uint32_t f3(uint32_t b, uint32_t c, uint32_t d)
{
    return (b & c) | (b & d) | (c & d);
}

} // anonymous namespace

Sha1::Sha1()
{
    initialize();
}

QByteArray Sha1::hashFinal()
{
    // save old hash if buffer is partially filled
    std::array<quint32, NUM_HASH_VALUES> oldHash{m_hash};
//    for (quint32 i = 0; i < NUM_HASH_VALUES; i++)
//        oldHash[i] = m_hash[i];

    // process remaining bytes
    processBuffer();

    QByteArray qba;
    qba.reserve(m_hashSize/std::numeric_limits<quint8>::digits);
//    const char* bytes = reinterpret_cast<const char*>(&m_hash[0]);
//    QByteArray qba = QByteArray(bytes, m_hashSize / std::numeric_limits<quint8>::digits);
    for (quint32 i = 0; i < NUM_HASH_VALUES; i++)
    {
//        qba.append(QByteArray::number(m_hash[i]));
        qba.append(static_cast<char>((m_hash[i] >> 24) & 0xFF));
        qba.append(static_cast<char>((m_hash[i] >> 16) & 0xFF));
        qba.append(static_cast<char>((m_hash[i] >>  8) & 0xFF));
        qba.append(static_cast<char>( m_hash[i]        & 0xFF));
//        const char * bytes = reinterpret_cast<const char*>(&m_hash[i]);
//        qba.append(bytes, sizeof(quint32));
    }

    // TODO: Validate code to transform m_hash to a QByteArray
//    for (const auto &hash: m_hash)
//        qba.append(QByteArray::number(hash));

    // restore old hash
    m_hash = oldHash;

    return qba;
}

quint32 Sha1::hashSize()
{
    return m_hashSize;
}

void Sha1::initialize()
{
    m_hashValue.clear();
    m_numBytes   = 0;
    m_bufferSize = 0;

    // according to RFC 1321
    m_hash[0] = UINT32_C(0x67452301);
    m_hash[1] = UINT32_C(0xefcdab89);
    m_hash[2] = UINT32_C(0x98badcfe);
    m_hash[3] = UINT32_C(0x10325476);
    m_hash[4] = UINT32_C(0xc3d2e1f0);
}

void Sha1::hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex)
{
    const quint8* current = static_cast<const quint8*>(data) + startIndex;
    quint64 numBytes = dataLength;

    if (m_bufferSize > 0)
    {
        while (numBytes > 0 && m_bufferSize < BLOCK_SIZE)
        {
            m_buffer[m_bufferSize++] = *current++;
            numBytes--;
        }
    }

    // full buffer
    if (m_bufferSize == BLOCK_SIZE)
    {
        processBlock(m_buffer.data());
        m_numBytes  += BLOCK_SIZE;
        m_bufferSize = 0;
    }

    // no more data ?
    if (numBytes == 0)
        return;

    // process full blocks
    while (numBytes >= BLOCK_SIZE)
    {
        processBlock(current);
        current    += BLOCK_SIZE;
        m_numBytes += BLOCK_SIZE;
        numBytes   -= BLOCK_SIZE;
    }

    // keep remaining bytes in buffer
    while (numBytes > 0)
    {
        m_buffer[m_bufferSize++] = *current++;
        numBytes--;
    }
}

void Sha1::processBlock(const void *data)
{
    // get last hash
    quint32 a = m_hash[0];
    quint32 b = m_hash[1];
    quint32 c = m_hash[2];
    quint32 d = m_hash[3];
    quint32 e = m_hash[4];

    // data represented as 16x 32-bit words
    const quint32* input = static_cast<const quint32*>(data);

    // convert to big endian
    std::array<quint32, 80> words;

#if defined(Q_BYTE_ORDER) && (Q_BYTE_ORDER != 0) && (Q_BYTE_ORDER == Q_BIG_ENDIAN)
    std::copy(input, input + 16, std::begin(words));
#else
    // convert to big endian
    std::transform(input, input + 16, std::begin(words),
                   [](const quint32 &b) -> quint32 { return QHashlib::swap(b); });
#endif

    // extend to 80 words
    for (int i = 16; i < 80; i++)
        words[i] = ROTATELEFT_M(words[i-3] ^ words[i-8] ^ words[i-14] ^ words[i-16], 1);

    // first round
    for (int i = 0; i < 4; i++)
    {
        int offset = 5*i;
        e += ROTATELEFT_M(a,5) + f1(b,c,d) + words[offset  ] + 0x5a827999; b = ROTATELEFT_M(b,30);
        d += ROTATELEFT_M(e,5) + f1(a,b,c) + words[offset+1] + 0x5a827999; a = ROTATELEFT_M(a,30);
        c += ROTATELEFT_M(d,5) + f1(e,a,b) + words[offset+2] + 0x5a827999; e = ROTATELEFT_M(e,30);
        b += ROTATELEFT_M(c,5) + f1(d,e,a) + words[offset+3] + 0x5a827999; d = ROTATELEFT_M(d,30);
        a += ROTATELEFT_M(b,5) + f1(c,d,e) + words[offset+4] + 0x5a827999; c = ROTATELEFT_M(c,30);
    }

    // second round
    for (int i = 4; i < 8; i++)
    {
        int offset = 5*i;
        e += ROTATELEFT_M(a,5) + f2(b,c,d) + words[offset  ] + 0x6ed9eba1; b = ROTATELEFT_M(b,30);
        d += ROTATELEFT_M(e,5) + f2(a,b,c) + words[offset+1] + 0x6ed9eba1; a = ROTATELEFT_M(a,30);
        c += ROTATELEFT_M(d,5) + f2(e,a,b) + words[offset+2] + 0x6ed9eba1; e = ROTATELEFT_M(e,30);
        b += ROTATELEFT_M(c,5) + f2(d,e,a) + words[offset+3] + 0x6ed9eba1; d = ROTATELEFT_M(d,30);
        a += ROTATELEFT_M(b,5) + f2(c,d,e) + words[offset+4] + 0x6ed9eba1; c = ROTATELEFT_M(c,30);
    }

    // third round
    for (int i = 8; i < 12; i++)
    {
        int offset = 5*i;
        e += ROTATELEFT_M(a,5) + f3(b,c,d) + words[offset  ] + 0x8f1bbcdc; b = ROTATELEFT_M(b,30);
        d += ROTATELEFT_M(e,5) + f3(a,b,c) + words[offset+1] + 0x8f1bbcdc; a = ROTATELEFT_M(a,30);
        c += ROTATELEFT_M(d,5) + f3(e,a,b) + words[offset+2] + 0x8f1bbcdc; e = ROTATELEFT_M(e,30);
        b += ROTATELEFT_M(c,5) + f3(d,e,a) + words[offset+3] + 0x8f1bbcdc; d = ROTATELEFT_M(d,30);
        a += ROTATELEFT_M(b,5) + f3(c,d,e) + words[offset+4] + 0x8f1bbcdc; c = ROTATELEFT_M(c,30);
    }

    // fourth round
    for (int i = 12; i < 16; i++)
    {
        int offset = 5*i;
        e += ROTATELEFT_M(a,5) + f2(b,c,d) + words[offset  ] + 0xca62c1d6; b = ROTATELEFT_M(b,30);
        d += ROTATELEFT_M(e,5) + f2(a,b,c) + words[offset+1] + 0xca62c1d6; a = ROTATELEFT_M(a,30);
        c += ROTATELEFT_M(d,5) + f2(e,a,b) + words[offset+2] + 0xca62c1d6; e = ROTATELEFT_M(e,30);
        b += ROTATELEFT_M(c,5) + f2(d,e,a) + words[offset+3] + 0xca62c1d6; d = ROTATELEFT_M(d,30);
        a += ROTATELEFT_M(b,5) + f2(c,d,e) + words[offset+4] + 0xca62c1d6; c = ROTATELEFT_M(c,30);
    }

    // update hash
    m_hash[0] += a;
    m_hash[1] += b;
    m_hash[2] += c;
    m_hash[3] += d;
    m_hash[4] += e;
}

void Sha1::processBuffer()
{
    // the input bytes are considered as bits strings, where the first bit is the most significant bit of the byte

    // - append "1" bit to message
    // - append "0" bits until message length in bit mod 512 is 448
    // - append length as 64 bit integer

    // number of bits
    quint64 paddedLength = m_bufferSize * 8;

    // plus one bit set to 1 (always appended)
    paddedLength++;

    // number of bits must be (numBits % 512) = 448
    quint64 lower11Bits = paddedLength & 511;
    if (lower11Bits <= 448)
        paddedLength +=       448 - lower11Bits;
    else
        paddedLength += 512 + 448 - lower11Bits;

    // convert from bits to bytes
    paddedLength /= 8;

    // only needed if additional data flows over into a second block
    std::array<quint8, BLOCK_SIZE> extra;

    // append a "1" bit, 128 => binary 10000000
    if (m_bufferSize < BLOCK_SIZE)
        m_buffer[m_bufferSize] = 128;
    else
        extra[0] = 128;

    quint64 i;
    for (i = m_bufferSize + 1; i < BLOCK_SIZE; i++)
        m_buffer[i] = 0;
    for (; i < paddedLength; i++)
        extra[i - BLOCK_SIZE] = 0;

    // add message length in bits as 64 bit number
    quint64 msgBits = 8 * (m_numBytes + m_bufferSize);

    // find right position
    std::array<quint8, BLOCK_SIZE>::iterator addLength;
    if (paddedLength < BLOCK_SIZE)
        addLength = m_buffer.begin() + paddedLength;
    else
        addLength = extra.begin() + paddedLength - BLOCK_SIZE;

    // must be big endian
    *addLength++ = static_cast<quint8>((msgBits >> 56) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 48) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 40) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 32) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 24) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >> 16) & 0xFF);
    *addLength++ = static_cast<quint8>((msgBits >>  8) & 0xFF);
    *addLength   = static_cast<quint8>( msgBits        & 0xFF);

    // process blocks
    processBlock(m_buffer.data());

    // flowed over into a second block ?
    if (paddedLength > BLOCK_SIZE)
        processBlock(extra.data());
}

} // namespace cryptographic
} // namespace QHashlib

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
#ifndef XXHASH64_HPP
#define XXHASH64_HPP

#include <hashalgorithm.hpp>
#include <array>

/// XXHash (64 bit), based on Yann Collet's descriptions, see http://cyan4973.github.io/xxHash/
/// Code adapted from Stephan Brumme's version: http://create.stephan-brumme.com/xxhash/
/// Note: Code is NOT endian-aware !
namespace QHashlib { namespace noncryptographic {

class XxHash64 : public HashAlgorithm
{
public:
    XxHash64(const quint64 &seed = 0);

    virtual QByteArray hashFinal() override;
    virtual quint32 hashSize() override;
    virtual void initialize() override;

protected:
    virtual void hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex) override;

private:
    static const quint32 m_hashSize = std::numeric_limits<quint32>::digits;

    /// magic constants
    static const quint64 Prime1 = UINT64_C(11400714785074694791);
    static const quint64 Prime2 = UINT64_C(14029467366897019727);
    static const quint64 Prime3 =  UINT64_C(1609587929392839161);
    static const quint64 Prime4 =  UINT64_C(9650029242287828579);
    static const quint64 Prime5 =  UINT64_C(2870177450012600261);

    /// temporarily store up to 31 bytes between multiple add() calls
    static const quint64 MaxBufferSize = 31+1;

    std::array<quint64, 4> m_state;
    std::array<quint8, MaxBufferSize> m_buffer;
    quint32  m_bufferSize;
    quint64  m_totalLength;
    quint64  m_seed;

    /// process a single 64 bit value
    quint64 processSingle(const quint64 &previous, const quint64 &input);

    /// process a block of 4x4 bytes, this is the main part of the XXHash32 algorithm
    void process(const void* data, quint64 &state0, quint64 &state1, quint64 &state2, quint64 &state3);
};

} // namespace noncryptographic
} // namespace QHashlib

#endif // XXHASH64_HPP

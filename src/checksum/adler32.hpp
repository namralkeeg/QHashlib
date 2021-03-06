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
#ifndef ADLER32_HPP
#define ADLER32_HPP

#include <hashalgorithm.hpp>
#include <limits>

namespace QHashlib { namespace checksum {

class Adler32 : public HashAlgorithm
{
public:
    Adler32();

    virtual QByteArray hashFinal() override;
    virtual quint32 hashSize() override;
    virtual void initialize() override;

protected:
    virtual void hashCore(const void *data, const quint64 &dataLength, const quint64 &startIndex) override;

private:
    static const quint32 m_hashSize = std::numeric_limits<quint32>::digits;
    const quint32 m_modAdler = UINT32_C(65521); // largest prime smaller than 65536
    const quint32 m_nmax     = UINT32_C(5552);
    quint32 m_hash           = UINT32_C(1);
};

} // namespace checksum
} // namespace QHashlib

#endif // ADLER32_HPP

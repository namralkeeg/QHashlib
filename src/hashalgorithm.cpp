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
#include <hashalgorithm.hpp>
#include <memory>

namespace QHashlib {

QByteArray HashAlgorithm::computeHash(const void *data, const quint64 &dataLength, const quint64 &index)
{
    initialize();
    hashCore(data, dataLength, index);
    m_hashValue = hashFinal();
    return m_hashValue;
}

QByteArray HashAlgorithm::computeHash(const void *data, const quint64 &dataLength)
{
    return computeHash(data, dataLength, 0);
}

QByteArray HashAlgorithm::computeHash(QIODevice &instream)
{
    if (!instream.isReadable())
    {
        return QByteArray();
    }
    else
    {
        instream.seek(0);
        qint64 bytesToRead = instream.size();
        qint64 blockSize = (m_blockSizeBuffer > bytesToRead) ? bytesToRead : m_blockSizeBuffer;

        // smart buffer array.
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(blockSize);
        initialize();

        qint64 numBytesRead = 0;
        while (bytesToRead > 0)
        {
            if (bytesToRead > blockSize)
                numBytesRead = instream.read(buffer.get(), blockSize);
            else
                numBytesRead = instream.read(buffer.get(), bytesToRead);

            // There was an error reading the IO device.
            if (numBytesRead <= 0)
                return QByteArray();

            hashCore(reinterpret_cast<const void*>(buffer.get()), numBytesRead, 0);
            bytesToRead -= numBytesRead;
        }

        m_hashValue = hashFinal();
        return m_hashValue;
    }
}

QByteArray HashAlgorithm::computeHash(const QByteArray &data, const qint32 &dataLength, const qint32 &index)
{
    return computeHash(data.data(), static_cast<quint64>(dataLength), static_cast<quint64>(index));
}

QByteArray HashAlgorithm::computeHash(const QByteArray &data, const qint32 &dataLength)
{
    return computeHash(data, dataLength, 0);
}

QByteArray HashAlgorithm::computeHash(const QByteArray &data)
{
    return computeHash(data, data.size());
}

QString HashAlgorithm::hashValueString(const bool &useUpperCase, const bool &insertSpaces)
{
    QByteArray temp(m_hashValue);

    if (useUpperCase)
        temp = temp.toUpper();

    if (!insertSpaces)
        temp = temp.toHex();
    else
        temp = temp.toHex(' ');

    return QString(temp);
}

QString HashAlgorithm::operator ()(const void *data, const quint64 &dataLength, const quint64 &index)
{
    computeHash(data, dataLength, index);
    return hashValueString();
}

QString HashAlgorithm::operator ()(const void *data, const quint64 &dataLength)
{
    computeHash(data, dataLength);
    return hashValueString();
}

QString HashAlgorithm::operator ()(const QString &text)
{
    QByteArray utf8Text = text.toUtf8();
    computeHash(utf8Text);
    return hashValueString();
}

QString HashAlgorithm::operator ()(const QByteArray &data)
{
    computeHash(data);
    return hashValueString();
}

HashAlgorithm::HashAlgorithm() : QObject()
{ }

void HashAlgorithm::hashCore(const void *data, const quint64 &dataLength)
{
    hashCore(data, dataLength, 0);
}

void HashAlgorithm::hashCore(const QByteArray &data, const qint32 &dataLength, const qint32 &starIndex)
{
    hashCore(reinterpret_cast<const void*>(data.data()), static_cast<quint64>(dataLength), static_cast<quint64>(starIndex));
}

void HashAlgorithm::hashCore(const QByteArray &data, const qint32 &dataLength)
{
    hashCore(data, dataLength, 0);
}

void HashAlgorithm::hashCore(const QByteArray &data)
{
    hashCore(data, data.size());
}

} // namespace QHashlib


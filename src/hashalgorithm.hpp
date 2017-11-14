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
#ifndef HASHALGORITHM_HPP
#define HASHALGORITHM_HPP

#include <QByteArray>
#include <QIODevice>
#include <QObject>
#include <QString>
#include <type_traits>

#ifndef HASH_BLOCK_BUFFER_SIZE
    // Block of bytes to process per file read.
    // each cycle processes about 1 MByte (divisible by 144 => improves Keccak/SHA3 performance)
    #define HASH_BLOCK_BUFFER_SIZE INT64_C(1032192) // 144 * 7 * 1024
#endif

namespace QHashlib {

class HashAlgorithm : public QObject
{
    Q_OBJECT

public:
    /// Virtual destructor
    virtual ~HashAlgorithm() { }

    /// compute Hash of a memory block returning the hash as a byte array.
    QByteArray computeHash(const void* data, const quint64 &dataLength, const quint64 &index);
    /// compute Hash of a memory block returning the hash as a byte array.
    QByteArray computeHash(const void* data, const quint64 &dataLength);
    /// Comput Hash of a stream
    QByteArray computeHash(QIODevice &instream);
    /// Compute hash of a byte array
    QByteArray computeHash(const QByteArray &data, const qint32 &dataLength, const qint32 &index);
    QByteArray computeHash(const QByteArray &data, const qint32 &dataLength);
    QByteArray computeHash(const QByteArray &data);

    /// This is called to finalize the hash computation.
    virtual QByteArray hashFinal() = 0;
    /// Size of the return hash in bits.
    virtual quint32 hashSize() = 0;
    /// Get the hash value as byte array.
    QByteArray hashValue() const { return m_hashValue; }
    /// Get the hash value as a hex string.
    QString hashValueString(const bool &useUpperCase = true, const bool &insertSpaces = false);
    /// Make sure everything is setup, or reset.
    virtual void initialize() = 0;

    /// compute Hash of a memory block returning the hash as a hex string.
    QString operator ()(const void* data, const quint64 &dataLength, const quint64 &index);
    /// compute Hash of a memory block returning the hash as a hex string.
    QString operator ()(const void* data, const quint64 &dataLength);
    /// compute Hash of a string, excluding final zero
    QString operator ()(const QString &text);
    /// compute hash of a byte array
    QString operator ()(const QByteArray &data);

protected:
    QByteArray m_hashValue;

    /// Protected ctor
    HashAlgorithm();

    /// Hashing function that does the work. Must be implemented in the derived class.
    virtual void hashCore(const void* data, const quint64 &dataLength, const quint64 &startIndex) = 0;
    /// Hashing helper functions that call the function implemented in a derived class.
    void hashCore(const void* data, const quint64 &dataLength);
    void hashCore(const QByteArray &data, const qint32 &dataLength, const qint32 &starIndex);
    void hashCore(const QByteArray &data, const qint32 &dataLength);
    void hashCore(const QByteArray &data);

private:
    static const qint64 m_blockSizeBuffer = HASH_BLOCK_BUFFER_SIZE;

    static_assert(std::is_same<quint8, unsigned char>::value,
                  "quint8 is required to be implemented as unsigned char!");
};

} // namespace QHashlib

#endif // HASHALGORITHM_HPP

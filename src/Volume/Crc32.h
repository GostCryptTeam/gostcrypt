/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Crc32
#define GST_HEADER_Encryption_Crc32

#include "Crypto/Crc.h"
#include "Core/Buffer.h"

namespace GostCrypt
{
namespace Volume
{
/**
 * @brief The Crc32 class implements a simple 32-bit error detecting code.
 * It it used only to check integrity of the header and to test the algorithms.
 */
class Crc32
{
 public:
    /**
     * @brief default initialisation sets the value so the final result is 0
     */
    Crc32() : CrcValue(0xffffFFFF) { }
    ~Crc32() { }

    /**
     * @brief this methods returns the INVERTED crc code resulting from the data processed in it
     * @return the crc32 value XORed
     */
    quint32 Get() const { return CrcValue ^ 0xffffFFFF; }

    /**
     * @brief this methos processes one byte of data to build the code
     * @param data the byte to process
     * @return the crc result of everything processed in this object
     */
    quint32 Process(quint8 data)
    {
        return CrcValue = crc_32_tab[(quint8)(CrcValue ^ data)] ^ (CrcValue >> 8);
    }

    /**
     * @brief ProcessBuffer processes a whole buffer into the crc code
     * @param buffer the buffer to process
     * @return the crc result of everything processed in this instance of Crc32
     */
    static quint32 ProcessBuffer(const BufferPtr& buffer)
    {
        return ::GetCrc32(const_cast<quint8*>(buffer.get()), static_cast<int>(buffer.size()));
    }

 protected:
    /**
     * @brief the crc32 value computed at every call of Process*
     */
    quint32 CrcValue;

 private:
    Crc32(const Crc32&);
    Crc32& operator= (const Crc32&);
};
}
}

#endif // GST_HEADER_Encryption_Crc32

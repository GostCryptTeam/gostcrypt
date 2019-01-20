/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionMode
#define GST_HEADER_Encryption_EncryptionMode

#include <QList>
#include <QSharedPointer>
#include "Crypto/Crypto.h"
#include "CipherAlgorithm.h"

namespace GostCrypt
{
namespace Volume
{

class EncryptionMode;
typedef QList < QSharedPointer <EncryptionMode> > EncryptionModeList;

/**
 * @brief The EncryptionMode class defines a way to encrypt a volume. Combined with a CipherAlgorithm, you get a whole EncryptionAlgorithm that can be used.
 * For Block Ciphers, you have to define how to encrypt each block. Default is ECB : you just encrypt each block independantly. It's a very bad practice since two same origin block will give the same ciphered blocks.
 * This class is just the template for a mode (same as CipherAlgorithms for ciphers).
 */
class EncryptionMode
{
 public:
    virtual ~EncryptionMode();

    /**
     * @brief Decrypt is a function to decrypt a simple block without any notion of sectorIndex or SectorSize
     * @param data pointer to the data area to decrypt
     * @param length size of the data to decrypt
     * @warning This function should only be used in specific cases where the position of the data is not relevant. Use DecryptSectors instead.
     */
    virtual void Decrypt(quint8* data, quint64 length) const = 0;

    /**
     * @brief DecryptSectors decrypts a whole range of sectors.
     * @param data the pointer to the area to decrypt
     * @param sectorIndex the index of the block to decrypt
     * @param sectorCount number of sectors to decrypt
     * @param sectorSize size of each sector to decrypt
     */
    virtual void DecryptSectors(quint8* data, quint64 sectorIndex, quint64 sectorCount,
                                size_t sectorSize) const;

    /**
     * @brief DecryptSectorsCurrentThread decrypts a whole range of sectors.
     * @param data the pointer to the area to decrypt
     * @param sectorIndex the index of the block to decrypt
     * @param sectorCount number of sectors to decrypt
     * @param sectorSize size of each sector to decrypt
     * @warning This function is Single-Threaded and used by EncryptionThreadPool to divise the workload.
     * @warning This function should only be used in specific cases where We don't want tu use at all the class EncryptionThreadPool
     */
    virtual void DecryptSectorsCurrentThread(quint8* data, quint64 sectorIndex, quint64 sectorCount,
            size_t sectorSize) const = 0;

    /**
     * @brief Encrypt is a function to encrypt a simple block without any notion of sectorIndex or SectorSize
     * @param data pointer to the data area to encrypt
     * @param length size of the data to encrypt
     * @warning This function should only be used in specific cases where the position of the data is not relevant. Use EncryptSectors instead.
     */
    virtual void Encrypt(quint8* data, quint64 length) const = 0;

    /**
     * @brief EncryptSectors encrypts a whole range of sectors.
     * @param data the pointer to the area to encrypt
     * @param sectorIndex the index of the block to encrypt
     * @param sectorCount number of sectors to encrypt
     * @param sectorSize size of each sector to encrypt
     */
    virtual void EncryptSectors(quint8* data, quint64 sectorIndex, quint64 sectorCount,
                                size_t sectorSize) const;

    /**
     * @brief EncryptSectorsCurrentThread encrypts a whole range of sectors.
     * @param data the pointer to the area to encrypt
     * @param sectorIndex the index of the block to encrypt
     * @param sectorCount number of sectors to encrypt
     * @param sectorSize size of each sector to encrypt
     * @warning This function is Single-Threaded and used by EncryptionThreadPool to divise the workload.
     * @warning This function should only be used in specific cases where We don't want tu use at all the class EncryptionThreadPool
     */
    virtual void EncryptSectorsCurrentThread(quint8* data, quint64 sectorIndex, quint64 sectorCount,
            size_t sectorSize) const = 0;

    /**
     * @brief GetKeySize returns the keysize of all the embedded ciphers
     * @return the sum of keysizes of the embedded algorithms
     */
    virtual size_t GetKeySize() const = 0;

    /**
     * @brief GetName returns the name of the encryption mode
     * @return the name
     */
    virtual QString GetName() const = 0;

    /**
     * @brief GetNew returns a new Encryption mode of the child type
     * @return a new empty EncrytpionModeXXX
     */
    virtual QSharedPointer <EncryptionMode> GetNew() const = 0;

    /**
     * @brief SetKey sets the key for the embedded ciphers
     * @param key the combination of all needed keys as a long buffer
     */
    virtual void SetKey(const BufferPtr& key) = 0;

    /**
     * @brief SetCiphers sets the embedded Ciphers. This Method must be called before any action.
     * @param ciphers the wanted ciphers for this Encryption Mode
     */
    virtual void SetCiphers(const CipherAlgorithmList& ciphers) { Ciphers = ciphers; }

    /**
     * @brief GetCiphers returns the cipher list associated with this encryption mode
     * @return the cipher list
     * @warning This does not return a copy of the list and should be used with care
     */
    virtual const CipherAlgorithmList GetCiphers() { return Ciphers; }
 protected:
    EncryptionMode();

    /**
     * @brief Ciphers the list of ciphers used in this mode when encrypting
     */
    CipherAlgorithmList Ciphers;

    /**
     * @brief KeySet variable to check if the Cipher Keys have been set or not
     * @warning This variable is not used
     * @todo This variable is not used
     */
    bool KeySet;

    /**
     * @brief SectorOffset is the starting offset for the mode.
     * @todo Completely useless right now since we're using XTS only.
     */
    quint64 SectorOffset;

 private:
    EncryptionMode(const EncryptionMode&);
    EncryptionMode& operator= (const EncryptionMode&);
};
}
}

#endif // GST_HEADER_Encryption_EncryptionMode

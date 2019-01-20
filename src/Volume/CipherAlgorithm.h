/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Ciphers
#define GST_HEADER_Encryption_Ciphers

#include "Core/Buffer.h"
#include <QList>
#include <QSharedPointer>

namespace GostCrypt
{
namespace Volume
{

class CipherAlgorithm;
typedef QList < QSharedPointer <CipherAlgorithm> > CipherAlgorithmList;

/**
 * @brief Virtual Class setting a template for Cipher Algorithms
 *
 */
class CipherAlgorithm
{
 public:
    virtual ~CipherAlgorithm();

     /**
     * @brief Method to decrypt N blocks of data from the given pointer
     *
     * @param data Pointer to the area to decrypt
     * @param blockCount Number of block of size getBlockSize() to decrypt
     *
     * @warning data content is overriden
     * @warning data must be the right size to avoid buffer overflows (blockCount * getBlockSize())
     */
    virtual void DecryptBlocks(quint8* data, size_t blockCount) const;

    /**
     * @brief Method to encrypt a block of data from the given pointer
     *
     * @param data Pointer to the area of size getBlockSize() to encrypt
     *
     * @warning data content is overriden
     * @warning data must be the right size to avoid buffer overflows (getBlockSize())
     */
    virtual void EncryptBlock(quint8* data) const;

    /**
     * @brief Method to encrypt N blocks of data from the given pointer
     *
     * @param data Pointer to the area to encrypt
     * @param blockCount Number of block of size getBlockSize() to encrypt
     *
     * @warning data content is overriden
     * @warning data must be the right size to avoid buffer overflows (blockCount * getBlockSize())
     */
    virtual void EncryptBlocks(quint8* data, size_t blockCount) const;

    /**
     * @brief Method to get the size of a single block that can be encrypted / decrypted using this algorithm
     * @return the minimal block size to encrypt or decrypt
     */
    virtual size_t GetBlockSize() const = 0;

    /**
     * @brief Method to get the required key size of the algorithm
     * @return the key size of the algorithm
     */
    virtual size_t GetKeySize() const = 0;

    /**
     * @brief Method to get the name of the algorithm
     * @return Name of the algorithm as a QString
     */
    virtual QString GetName() const = 0;

    /**
     * @brief Method to get the description of the algorithm
     * @return Short description of the algorithm as a QString
     */
    virtual QString GetDescription() const = 0;

    /**
     * @brief GetNew just returns a new Instance of the Algorithm as a Shared Pointer
     * @return a QSharedPointer on a initialised algorithm as a CipherAlgorithm
     */
    virtual QSharedPointer <CipherAlgorithm> GetNew() const = 0;

    /**
     * @brief Sets the key that will be used in the algorithm
     * @param key the key of size GetKeySize()
     */
    virtual void SetKey(const BufferPtr& key);

    /**
     * @brief XorCipherKey XORs the Sheduled key with the content of data of length len and returns the result in buffer ks
     * @param ks output buffer to stored the XORed result
     * @param data data to XOR the key with
     * @param len length of data in Bytes
     * @note This method is only used in XTS encryption mode
     */
    virtual void XorCipherKey(quint8* ks, quint8* data, int len) const = 0;

    /**
     * @brief EncryptWithKS encrypts one block of the buffer with a custom key
     * @param data the data of size getBlockSize() to encrypt
     * @param ks The key to use instead of SheduledKey
     * @note This method is only used in XTS encryption mode (with a XORed key obtained with XorCipherKey)
     * @warning ks isn't just a random string of size getScheduledKeySize(), but an actual structure depending on the algorithm used
     */
    virtual void EncryptWithKS(quint8* data, quint8* ks) const = 0;

    /**
     * @brief DecryptWithKS decrypts one block of the buffer with a custom key
     * @param data the data of size getBlockSize() to decrypt
     * @param ks The key to use instead of SheduledKey
     * @note This method is only used in XTS encryption mode (with a XORed key obtained with XorCipherKey)
     * @warning ks isn't just a random string of size getScheduledKeySize(), but an actual structure depending on the algorithm used
     */
    virtual void DecryptWithKS(quint8* data, quint8* ks) const = 0;

    /**
     * @brief CopyCipherKey Copies the Sheduled Key inside Buffer ks.
     * @param ks the target buffer to copy the key
     * @todo The content of a SecureBuffer should never be copied to an unknown pointer
     */
    virtual void CopyCipherKey(quint8* ks) const = 0;

    /**
     * @brief GetScheduledKeySize returns the size of the Sheduled Key.
     * This size is completely different from the Key size, because it can contain more complex structures like a Dynamic sBox.
     * @return The Sheduled Key size
     */
    virtual size_t GetScheduledKeySize() const = 0;

 protected:
    CipherAlgorithm();

    /**
     * @brief Decrypt is the low level Version of DecryptBlock and depends on the implementation of the algorithm
     * @param data Pointer to the area of size getBlockSize() to decrypt
     *
     * @warning data content is overriden
     * @warning data must be the right size to avoid buffer overflows (getBlockSize())
     */
    virtual void Decrypt(quint8* data) const = 0;

    /**
     * @brief Encrypt is the low level Version of EncryptBlock and depends on the implementation of the algorithm
     * @param data Pointer to the area of size getBlockSize() to encrypt
     *
     * @warning data content is overriden
     * @warning data must be the right size to avoid buffer overflows (getBlockSize())
     */
    virtual void Encrypt(quint8* data) const = 0;

    /**
     * @brief SetCipherKey sets the Sheduled Key structure from the given Key
     * @param key the "normal" key to use (hashed password of size GetKeySize() )
     */
    virtual void SetCipherKey(const quint8* key) = 0;

    /**
     * @brief Initialized is false until the Keys have not been set.
     * Once the algorithm is ready to be used, this var is set to true.
     */
    bool Initialized;

    /**
     * @brief The Key entered and used for decryption
     */
    SecureBuffer Key;

    /**
     * @brief The actual key used in the algorithm, derived from Key with SetCipherKey().
     * This is not just a simple Buffer but a structure depending on the algorithm
     */
    SecureBuffer ScheduledKey;

 private:
    CipherAlgorithm(const CipherAlgorithm&);
    CipherAlgorithm& operator= (const CipherAlgorithm&);
};

}
}
#endif // GST_HEADER_Encryption_Ciphers

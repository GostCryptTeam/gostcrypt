#ifndef VOLUMEEXCEPTION_H
#define VOLUMEEXCEPTION_H

#include "Core/GostCryptException.h"

namespace GostCrypt
{
namespace Volume
{

void initVolumeException();

class VolumeException : public GostCryptException
{
 public:
    VolumeException() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeException, GostCryptException, "")

    DEC_SERIALIZABLE(VolumeException);
};


#define EncryptionTestFailedException() GostCrypt::Volume::EncryptionTestFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class EncryptionTestFailed : public VolumeException
{
 public:
    EncryptionTestFailed() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    EncryptionTestFailed(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(EncryptionTestFailed, VolumeException, "Encryption Test Failed")

    DEC_SERIALIZABLE(EncryptionTestFailed);
};

#define CipherAlgorithmNotInitializedException() GostCrypt::Volume::CipherAlgorithmNotInitialized(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class CipherAlgorithmNotInitialized : public VolumeException
{
 public:
    CipherAlgorithmNotInitialized() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    CipherAlgorithmNotInitialized(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(CipherAlgorithmNotInitialized, VolumeException, "Cipher key not set")

    DEC_SERIALIZABLE(CipherAlgorithmNotInitialized);
};

#define EncryptionAlgorithmNotInitializedException() GostCrypt::Volume::EncryptionAlgorithmNotInitialized(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class EncryptionAlgorithmNotInitialized : public VolumeException
{
 public:
    EncryptionAlgorithmNotInitialized() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    EncryptionAlgorithmNotInitialized(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(EncryptionAlgorithmNotInitialized, VolumeException, "Ciphers not set")

    DEC_SERIALIZABLE(EncryptionAlgorithmNotInitialized);
};

#define EncryptionModeNotInitializedException() GostCrypt::Volume::EncryptionModeNotInitialized(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class EncryptionModeNotInitialized : public VolumeException
{
 public:
    EncryptionModeNotInitialized() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    EncryptionModeNotInitialized(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(EncryptionModeNotInitialized, VolumeException, "Ciphers not set")

    DEC_SERIALIZABLE(EncryptionModeNotInitialized);
};

#define BufferAlreadyFreedException() GostCrypt::Volume::BufferAlreadyFreed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class BufferAlreadyFreed : public VolumeException
{
 public:
    BufferAlreadyFreed() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    BufferAlreadyFreed(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(BufferAlreadyFreed, VolumeException, "The buffer has already been freed.")

    DEC_SERIALIZABLE(BufferAlreadyFreed);
};

#define VolumeNotOpenException() GostCrypt::Volume::VolumeNotOpen(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class VolumeNotOpen : public VolumeException
{
 public:
    VolumeNotOpen() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeNotOpen(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeNotOpen, VolumeException, "Trying to use the volume but it is not open.")

    DEC_SERIALIZABLE(VolumeNotOpen);
};

#define VolumeReadOnlyException() GostCrypt::Volume::VolumeReadOnly(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class VolumeReadOnly : public VolumeException
{
 public:
    VolumeReadOnly() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeReadOnly(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeReadOnly, VolumeException, "The volume is opened in read-only.")

    DEC_SERIALIZABLE(VolumeReadOnly);
};

#define VolumeProtectedException() GostCrypt::Volume::VolumeProtected(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class VolumeProtected : public VolumeException
{
 public:
    VolumeProtected() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeProtected(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeProtected, VolumeException, "Trying to write in a protected area.")

    DEC_SERIALIZABLE(VolumeProtected);
};

#define VolumeCorruptedException() GostCrypt::Volume::VolumeCorrupted(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class VolumeCorrupted : public VolumeException
{
 public:
    VolumeCorrupted() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeCorrupted(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeCorrupted, VolumeException, "The volume is corrupted")

    DEC_SERIALIZABLE(VolumeCorrupted);
};

#define PasswordOrKeyfilesIncorrectException(volumePath) GostCrypt::Volume::PasswordOrKeyfilesIncorrect(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
class PasswordOrKeyfilesIncorrect : public VolumeException
{
 public:
    PasswordOrKeyfilesIncorrect() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     * @param volumePath QFileInfo corresponding to the volume to be mounted
     */
    PasswordOrKeyfilesIncorrect(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : VolumeException(fonction,
                filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(PasswordOrKeyfilesIncorrect, VolumeException, "The given password or keyfiles for the volume " + volumePath.canonicalFilePath() + "are incorrect.")
    QFileInfo volumePath; /**< QFileInfo corresponding to the volume to be mounted */
    DEC_SERIALIZABLE(PasswordOrKeyfilesIncorrect);
};

#define ProtectionPasswordOrKeyfilesIncorrectException(volumePath) GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
class ProtectionPasswordOrKeyfilesIncorrect : public VolumeException
{
 public:
    ProtectionPasswordOrKeyfilesIncorrect() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     * @param volumePath QFileInfo corresponding to the volume to be mounted
     */
    ProtectionPasswordOrKeyfilesIncorrect(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : VolumeException(fonction,
                filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(ProtectionPasswordOrKeyfilesIncorrect, VolumeException, "The given password or keyfiles for hidden volume ("+volumePath.canonicalFilePath()+") are incorrect.")
    QFileInfo volumePath; /**< QFileInfo corresponding to the volume to be mounted */
    DEC_SERIALIZABLE(ProtectionPasswordOrKeyfilesIncorrect);
};

#define VolumeVersionNotCompatibleException(comment) GostCrypt::Volume::VolumeVersionNotCompatible(__PRETTY_FUNCTION__, __FILE__, __LINE__, comment);
class VolumeVersionNotCompatible : public VolumeException
{
 public:
    VolumeVersionNotCompatible() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    VolumeVersionNotCompatible(QString fonction, QString filename, quint32 line, QString c) : VolumeException(fonction,
                filename, line), comment(c) {}
    DEF_EXCEPTION_WHAT(VolumeVersionNotCompatible, VolumeException, "The volume version is not compatible with this version of GostCrypt ("+comment+")")
    QString comment;
    DEC_SERIALIZABLE(VolumeVersionNotCompatible);
};

#define FailedResetTimestampsException() GostCrypt::Volume::FailedResetTimestamps(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class FailedResetTimestamps : public VolumeException
{
 public:
    FailedResetTimestamps() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    FailedResetTimestamps(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(FailedResetTimestamps, VolumeException, "Failed to reset the timestamp of the volume file.")

    DEC_SERIALIZABLE(FailedResetTimestamps);
};

#define FailedGetSectorSizeException() GostCrypt::Volume::FailedGetSectorSize(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class FailedGetSectorSize : public VolumeException
{
 public:
    FailedGetSectorSize() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    FailedGetSectorSize(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(FailedGetSectorSize, VolumeException, "Failed to get the sector size")

    DEC_SERIALIZABLE(FailedGetSectorSize);
};

#define FailedGetTimestampsException() GostCrypt::Volume::FailedGetTimestamps(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class FailedGetTimestamps : public VolumeException
{
 public:
    FailedGetTimestamps() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    FailedGetTimestamps(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(FailedGetTimestamps, VolumeException, "Failed to get the timestamps of the volume file.")

    DEC_SERIALIZABLE(FailedGetTimestamps);
};

#define DataNotMutableException() GostCrypt::Volume::DataNotMutable(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class DataNotMutable : public VolumeException
{
 public:
    DataNotMutable() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    DataNotMutable(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(DataNotMutable, VolumeException, "The buffer was created with a cont buffer but is not const itself and you're trying to access it.")

    DEC_SERIALIZABLE(DataNotMutable);
};

#define IncorrectCipherDataLengthException() GostCrypt::Volume::IncorrectCipherDataLength(__PRETTY_FUNCTION__, __FILE__, __LINE__);
class IncorrectCipherDataLength : public VolumeException
{
 public:
    IncorrectCipherDataLength() {}
    /**
     * @brief Base class for all exception concerning Volume module
     *
     * @param fonction Name of the function where the exception was thrown
     * @param filename Path of the file where the exception was thrown
     * @param line Line of the file where the exception was thrown
     */
    IncorrectCipherDataLength(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(IncorrectCipherDataLength, VolumeException, "The size of the buffer to cipher is not multiple of block size")

    DEC_SERIALIZABLE(IncorrectCipherDataLength);
};

}
}

SERIALIZABLE(GostCrypt::Volume::VolumeException)
SERIALIZABLE(GostCrypt::Volume::EncryptionTestFailed)
SERIALIZABLE(GostCrypt::Volume::CipherAlgorithmNotInitialized)
SERIALIZABLE(GostCrypt::Volume::EncryptionAlgorithmNotInitialized)
SERIALIZABLE(GostCrypt::Volume::EncryptionModeNotInitialized)
SERIALIZABLE(GostCrypt::Volume::BufferAlreadyFreed)
SERIALIZABLE(GostCrypt::Volume::VolumeNotOpen)
SERIALIZABLE(GostCrypt::Volume::VolumeReadOnly)
SERIALIZABLE(GostCrypt::Volume::VolumeProtected)
SERIALIZABLE(GostCrypt::Volume::VolumeCorrupted)
SERIALIZABLE(GostCrypt::Volume::PasswordOrKeyfilesIncorrect)
SERIALIZABLE(GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect)
SERIALIZABLE(GostCrypt::Volume::VolumeVersionNotCompatible)
SERIALIZABLE(GostCrypt::Volume::FailedResetTimestamps)
SERIALIZABLE(GostCrypt::Volume::FailedGetSectorSize)
SERIALIZABLE(GostCrypt::Volume::FailedGetTimestamps)
SERIALIZABLE(GostCrypt::Volume::DataNotMutable)
SERIALIZABLE(GostCrypt::Volume::IncorrectCipherDataLength)

#endif // VOLUMEEXCEPTION_H

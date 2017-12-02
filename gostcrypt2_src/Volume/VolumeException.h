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
    DEF_EXCEPTION_WHAT(VolumeNotOpen, VolumeException, "The buffer has already been freed.")

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
    DEF_EXCEPTION_WHAT(VolumeReadOnly, VolumeException, "The buffer has already been freed.")

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
    DEF_EXCEPTION_WHAT(VolumeProtected, VolumeException, "The buffer has already been freed.")

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
    DEF_EXCEPTION_WHAT(VolumeCorrupted, VolumeException, "The buffer has already been freed.")

    DEC_SERIALIZABLE(VolumeCorrupted);
};

#define PasswordOrKeyfilesIncorrectException() GostCrypt::Volume::PasswordOrKeyfilesIncorrect(__PRETTY_FUNCTION__, __FILE__, __LINE__);
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
     */
    PasswordOrKeyfilesIncorrect(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(PasswordOrKeyfilesIncorrect, VolumeException, "The buffer has already been freed.")

    DEC_SERIALIZABLE(PasswordOrKeyfilesIncorrect);
};

#define ProtectionPasswordOrKeyfilesIncorrectException() GostCrypt::Volume::ProtectionPasswordOrKeyfilesIncorrect(__PRETTY_FUNCTION__, __FILE__, __LINE__);
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
     */
    ProtectionPasswordOrKeyfilesIncorrect(QString fonction, QString filename, quint32 line) : VolumeException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(ProtectionPasswordOrKeyfilesIncorrect, VolumeException, "The buffer has already been freed.")

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
    VolumeVersionNotCompatible(QString fonction, QString filename, quint32 line, QString comment) : VolumeException(fonction,
                filename, line), comment(comment) {}
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
    DEF_EXCEPTION_WHAT(FailedResetTimestamps, VolumeException, "The buffer has already been freed.")

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
    DEF_EXCEPTION_WHAT(FailedGetSectorSize, VolumeException, "The buffer has already been freed.")

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
    DEF_EXCEPTION_WHAT(FailedGetTimestamps, VolumeException, "The buffer has already been freed.")

    DEC_SERIALIZABLE(FailedGetTimestamps);
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


#endif // VOLUMEEXCEPTION_H

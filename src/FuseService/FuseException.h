#ifndef FUSEEXCEPTION_H
#define FUSEEXCEPTION_H
#include "Core/GostCryptException.h"

namespace GostCrypt
{
namespace FuseDriver
{
/**
 * @brief Initialize the FuseException class and subclasses to make them serializable across processes
 *
 */
void initFuseException();

/**
 * @brief Parent class of all Exceptions related to FuseService module
 */
class FuseException : public GostCryptException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     */
    FuseException() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    FuseException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(FuseException, GostCryptException, "")

    DEC_SERIALIZABLE(FuseException);
};

#define FuseTimeoutException(volumePath) FuseTimeout(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
/**
 * @brief Exception thrown when the Fuse mount is still not ready 10 seconds ater the fuse_main function was called
 *
 */
class FuseTimeout : public FuseException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     *
     */
    FuseTimeout() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param volumePath Path of the volume container the Fuse service was trying to mount
     */
    FuseTimeout(QString fonction, QString filename, quint32 line,
                QFileInfo volumePath) : FuseException(fonction, filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(FuseTimeout, FuseException,
                       "Fuseservice timed out while trying to mount " + volumePath.absoluteFilePath() + "\n")
 protected:
    QFileInfo volumePath; /**< Path of the volume container the Fuse service was trying to mount */
    DEC_SERIALIZABLE(FuseTimeout);
};

#define FuseControlFileAccessFailedException(fuseMountPoint) FuseControlFileAccessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__, fuseMountPoint);
/**
 * @brief Exception thrown when the Fuse module fail to access the control file within the fuse mount to write additionnal information on it
 *
 */
class FuseControlFileAccessFailed : public FuseException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     */
    FuseControlFileAccessFailed() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param fuseMountPoint Path of the directory where the fuse file system should be mounted
     */
    FuseControlFileAccessFailed(QString fonction, QString filename, quint32 line,
                                QFileInfo fuseMountPoint) : FuseException(fonction, filename, line),
        fuseMountPoint(fuseMountPoint) {}
    DEF_EXCEPTION_WHAT(FuseControlFileAccessFailed, FuseException,
                       "Fail to open control file to write in : " + fuseMountPoint.absoluteFilePath() + "\n")
 protected:
    QFileInfo fuseMountPoint; /**< Path of the directory where the fuse file system should be mounted */
    DEC_SERIALIZABLE(FuseControlFileAccessFailed);
};

#define FuseForkFailedException() FuseForkFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when the fuse module fail to fork in order to launch the fuse library process
 *
 */
class FuseForkFailed : public FuseException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     */
    FuseForkFailed() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    FuseForkFailed(QString fonction, QString filename, quint32 line) : FuseException(fonction, filename,
                line) {}
    DEF_EXCEPTION_WHAT(FuseForkFailed, FuseException,
                       "The FuseService was not able to fork successfully\n")
    DEC_SERIALIZABLE(FuseForkFailed);
};

#define VolumeNotOpenedYetException() VolumeNotOpenedYet(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when the fuse module try to access a volume file before the volume had been opened
 *
 */
class VolumeNotOpenedYet : public FuseException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     */
    VolumeNotOpenedYet() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    VolumeNotOpenedYet(QString fonction, QString filename, quint32 line) : FuseException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(VolumeNotOpenedYet, FuseException,
                       "Information about the volume were requested before the volume had been opened\n")
    DEC_SERIALIZABLE(VolumeNotOpenedYet);
};
#define FailedCreateFuseMountPointException(mountpoint) GostCrypt::FuseDriver::FailedCreateFuseMountPoint(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
/**
 * @brief Exception thrown when the directory where to mount the fuse file system can not be created
 *
 */
class FailedCreateFuseMountPoint : public FuseException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     *
     */
    FailedCreateFuseMountPoint() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param mountpoint Path of the directory that can not be created where the fuse file system should be mounted
     */
    FailedCreateFuseMountPoint(QString fonction, QString filename, quint32 line,
                               QFileInfo mountpoint) : FuseException(fonction, filename, line), mountpoint(mountpoint) {}
    DEF_EXCEPTION_WHAT(FailedCreateFuseMountPoint, FuseException,
                       "Creation of fuse mount point " + mountpoint.absoluteFilePath() + " failed\n")
 protected:
    QFileInfo mountpoint; /**< Path of the directory that can not be created where the fuse file system should be mounted */
    DEC_SERIALIZABLE(FailedCreateFuseMountPoint);
};
}
}

SERIALIZABLE(GostCrypt::FuseDriver::FuseException)
SERIALIZABLE(GostCrypt::FuseDriver::FuseTimeout)
SERIALIZABLE(GostCrypt::FuseDriver::FuseControlFileAccessFailed)
SERIALIZABLE(GostCrypt::FuseDriver::FuseForkFailed)
SERIALIZABLE(GostCrypt::FuseDriver::VolumeNotOpenedYet)
SERIALIZABLE(GostCrypt::FuseDriver::FailedCreateFuseMountPoint)

#endif // FUSEEXCEPTION_H

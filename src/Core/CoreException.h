#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include <QString>
#include <QVariant>
#include <QDebug>
#include "GostCryptException.h"

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Initialize the CoreException class and subclasses to make them serializable across processes
 */
void initCoreException();

/**
 * @brief Parent class of all Exceptions related to Core module
 *
 */
class CoreException : public GostCryptException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    CoreException() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    */
    CoreException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(CoreException, GostCryptException, "")

    DEC_SERIALIZABLE(CoreException);
};

#define DeviceNotMountedException(devicePath) GostCrypt::Core::DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
/**
 * @brief Exception thrown when the requested device is not mounted
 *
 */
class DeviceNotMounted : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    DeviceNotMounted() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param device QFileInfo corresponding to requested device
     */
    DeviceNotMounted(QString fonction, QString filename, quint32 line,
                     QFileInfo device) : CoreException(fonction, filename, line), device(device) {}
    DEF_EXCEPTION_WHAT(DeviceNotMounted, CoreException,
                       "The device " + device.absoluteFilePath() + " is not mounted.\n")
 protected:
    QFileInfo device; /**< QFileInfo corresponding to requested device */ //TODO remove pointer (QFileInfo is internally only a device pointer)

    DEC_SERIALIZABLE(DeviceNotMounted);
};

#define VolumeAlreadyMountedException(volumePath) GostCrypt::Core::VolumeAlreadyMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
/**
 * @brief Exception thrown when the requested action can not be performed because the volume is already mounted
 *
 */
class VolumeAlreadyMounted : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    VolumeAlreadyMounted() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param volumePath QFileInfo corresponding to the already mounted volume
     */
    VolumeAlreadyMounted(QString fonction, QString filename, quint32 line,
                         QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException,
                       "The volume " + volumePath.absoluteFilePath() + " is already mounted.\n")
 protected:
    QFileInfo volumePath; /**< QFileInfo corresponding to the already mounted volume */
    DEC_SERIALIZABLE(VolumeAlreadyMounted);
};

#define FailedOpenVolumeException(volumePath) GostCrypt::Core::FailedOpenVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
/**
 * @brief Exception thrown when the given volume file cannot be open
 *
 */
class FailedOpenVolume : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailedOpenVolume() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param volumePath QFileInfo corresponding to the volume to open
     */
    FailedOpenVolume(QString fonction, QString filename, quint32 line,
                     QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(FailedOpenVolume, CoreException,
                       "Opening of volume " + volumePath.absoluteFilePath() + " failed.\n")
 protected:
    QFileInfo volumePath; /**< QFileInfo corresponding to the volume to open */
    DEC_SERIALIZABLE(FailedOpenVolume);
};

#define IncorrectSudoPasswordException() GostCrypt::Core::IncorrectSudoPassword(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when the sudo command does not launch the desired command with root privileges, most likely because the password entered is incorrect
 *
 */
class IncorrectSudoPassword : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    IncorrectSudoPassword() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    IncorrectSudoPassword(QString fonction, QString filename, quint32 line) : CoreException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(IncorrectSudoPassword, CoreException,
                       "The given passwords are wrong, can't execute request.\n")
 protected:
    DEC_SERIALIZABLE(IncorrectSudoPassword);
};

#define ServiceProcessCrashedException() GostCrypt::Core::ServiceProcessCrashed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when a service child process launched by GostCrypt exit unexpectedly
 *
 */
class ServiceProcessCrashed : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    ServiceProcessCrashed() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    ServiceProcessCrashed(QString fonction, QString filename, quint32 line) : CoreException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(ServiceProcessCrashed, CoreException,
                       "The service process crashed unexpectedly.\n")
 protected:
    DEC_SERIALIZABLE(ServiceProcessCrashed);
};

#define MountPointUsedException(mountpoint) GostCrypt::Core::MountPointUsed(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
/**
 * @brief Exception thrown when the tested mountpoint is already used
 *
 */
class MountPointUsed : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    MountPointUsed() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param mountpoint QFileInfo correspponding to the tested mountpoint
     */
    MountPointUsed(QString fonction, QString filename, quint32 line,
                   QFileInfo mountpoint) : CoreException(fonction, filename, line),
        mountpoint(mountpoint) {}
    /**
     * @brief Return the QFileInfo correspponding to the tested mountpoint
     *
     * @return QFileInfo correspponding to the tested mountpoint
     */
    QFileInfo getMountpoint() { return mountpoint; }
    DEF_EXCEPTION_WHAT(MountPointUsed, CoreException,
                       "The directory " + mountpoint.absoluteFilePath() + " is already used\n")
 protected:
    QFileInfo mountpoint; /**< QFileInfo correspponding to the tested mountpoint */
    DEC_SERIALIZABLE(MountPointUsed);
};

/**
 * @brief Base Class for all exceptions related to MountFilesystemManager
 *
 */
class MountFilesystemManagerException : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    MountFilesystemManagerException() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param errorNumber Value of the errno variable after the failed system call
     * @param mountpoint QFileInfo corresponding to the used mountpoint
     */
    MountFilesystemManagerException(QString fonction, QString filename, quint32 line,
                                    quint32 errorNumber, QFileInfo mountpoint) : CoreException(fonction, filename,
                                                line), errorNumber(errorNumber), mountpoint(mountpoint) {}
    DEF_EXCEPTION_WHAT(MountFilesystemManagerException, CoreException, "")
 protected:
    qint32 errorNumber; /**< Value of the errno variable after the failed system call */
    QFileInfo mountpoint; /**< QFileInfo corresponding to the used mountpoint */

    DEC_SERIALIZABLE(MountFilesystemManagerException);
};

#define FailMountFilesystemException(errorNumber, mountpoint, devicePath, filesystemtype) GostCrypt::Core::FailMountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, errorNumber, mountpoint, devicePath, filesystemtype);
/**
 * @brief Exception thrown when the loop device corresponding to a GostCrypt volume failed to be mounted
 *
 */
class FailMountFilesystem : public MountFilesystemManagerException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailMountFilesystem() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param errorNumber Value of the errno variable after the failed system call
     * @param mountpoint QFileInfo corresponding to the used mountpoint
     * @param devicePath QFileInfo corresponding to the volume loop device used to try to mount
     * @param filesystemType Name of the filesystem type used to try to mount
     */
    FailMountFilesystem(QString fonction, QString filename, quint32 line, quint32 errorNumber,
                        QFileInfo mountpoint, QFileInfo devicePath,
                        QString filesystemtype) : MountFilesystemManagerException(fonction, filename, line, errorNumber,
                                    mountpoint), devicePath(devicePath), filesystemtype(filesystemtype) {}
    DEF_EXCEPTION_WHAT(FailMountFilesystem, MountFilesystemManagerException,
                       "Unable to mount " + devicePath.absoluteFilePath() + " in " + mountpoint.absoluteFilePath() +
                       " as " + filesystemtype + " (" + QString::number(errorNumber) + ":" + QString(strerror(
                                   errorNumber)) + ")\n")
 protected:
    QFileInfo devicePath; /**< QFileInfo corresponding to the volume file to mount */
    QString filesystemtype; /**< Name of the filesystem type used to try to mount */

    DEC_SERIALIZABLE(FailMountFilesystem);
};

#define FailFindFilesystemTypeException(devicePath) GostCrypt::Core::FailFindFilesystemType(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
/**
 * @brief Exception thrown when the filesystemtype of a loop device can not be determined
 *
 */
class FailFindFilesystemType : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailFindFilesystemType() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param devicePath QFileInfo coresponding to the loop device for which it tried to find the filesystem type
     */
    FailFindFilesystemType(QString fonction, QString filename, quint32 line,
                           QFileInfo devicePath) : CoreException(fonction, filename, line),
        devicePath(devicePath) {}
    DEF_EXCEPTION_WHAT(FailFindFilesystemType, CoreException,
                       "Unable to mount " + devicePath.absoluteFilePath() + ": Failed to find filesystem type.")
 protected:
    QFileInfo devicePath; /**< QFileInfo coresponding to the loop device for which it tried to find the filesystem type */

    DEC_SERIALIZABLE(FailFindFilesystemType);
};

#define FailUnmountFilesystemException(errorNumber, mountpoint) GostCrypt::Core::FailUnmountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, errorNumber, mountpoint);
/**
 * @brief Exception thrown when the loop device corresponding to a GostCrypt volume failed to be unmounted
 *
 */
class FailUnmountFilesystem : public MountFilesystemManagerException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailUnmountFilesystem() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param errorNumber Value of the errno variable after the failed system call
     * @param mountpoint QFileInfo corresponding to the used mountpoint
     */
    FailUnmountFilesystem(QString fonction, QString filename, quint32 line, quint32 errorNumber,
                          QFileInfo mountpoint) : MountFilesystemManagerException(fonction, filename, line,
                                      errorNumber, mountpoint) {}
    DEF_EXCEPTION_WHAT(FailUnmountFilesystem, MountFilesystemManagerException,
                       "Unable to unmount filesystem in " + mountpoint.absoluteFilePath() + "(" + QString(strerror(
                                   errorNumber)) + ")\n")
    DEC_SERIALIZABLE(FailUnmountFilesystem);
};

#define FailedAttachLoopDeviceException(imageFile) GostCrypt::Core::FailedAttachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, imageFile);
/**
 * @brief Exception thrown when the binding of a loop device to the volume image file fail
 *
 */
class FailedAttachLoopDevice : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailedAttachLoopDevice() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param imageFile QFileInfo corresponding to the volume image file (inside fuse mount)
     */
    FailedAttachLoopDevice(QString fonction, QString filename, quint32 line,
                           QFileInfo imageFile) : CoreException(fonction, filename, line),
        imageFile(imageFile) { }
    DEF_EXCEPTION_WHAT(FailedAttachLoopDevice, CoreException,
                       QStringLiteral("Unable to create loop device for image file ") + imageFile.absoluteFilePath() +
                       "\n")
 protected:
    QFileInfo imageFile; /**< QFileInfo corresponding to the volume image file (inside fuse mount) */

    DEC_SERIALIZABLE(FailedAttachLoopDevice);
};

#define FailedDetachLoopDeviceException(loopDevice) GostCrypt::Core::FailedDetachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, loopDevice);
/**
 * @brief Exception thrown when the unbinding of the loop device to the volume image file fail
 *
 */
class FailedDetachLoopDevice : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailedDetachLoopDevice() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param loopDevice QFileInfo corresponding to the loop device file binded to the volume image file
     */
    FailedDetachLoopDevice(QString fonction, QString filename, quint32 line,
                           QFileInfo loopDevice) : CoreException(fonction, filename, line),
        loopDevice(loopDevice) { }
    DEF_EXCEPTION_WHAT(FailedDetachLoopDevice, CoreException,
                       QStringLiteral("Unable to detach loop device ") + loopDevice.absoluteFilePath() + "\n")
 protected:
    QFileInfo loopDevice; /**< QFileInfo corresponding to the loop device file binded to the volume image file */

    DEC_SERIALIZABLE(FailedDetachLoopDevice);
};

#define VolumeNotMountedException(volumePath) GostCrypt::Core::VolumeNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
/**
 * @brief Exception thrown when trying to unmount a volume currently not mounted
 *
 */
class VolumeNotMounted : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    VolumeNotMounted() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param volumePath QFileInfo corresponding to the not mounted volume
     */
    VolumeNotMounted(QString fonction, QString filename, quint32 line,
                     QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
    DEF_EXCEPTION_WHAT(VolumeNotMounted, CoreException,
                       "The volume " + volumePath.absoluteFilePath() + " is not mounted.\n")
 protected:
    QFileInfo volumePath; /**< QFileInfo corresponding to the not mounted volume */
    DEC_SERIALIZABLE(VolumeNotMounted);
};

#define FormattingSubExceptionException(subExceptionString) GostCrypt::Core::FormattingSubException(__PRETTY_FUNCTION__, __FILE__, __LINE__, subExceptionString);
/**
 * @brief Exception thrown when an exception occur while dismounting the volume after formatting the loop device corresponding to the GostCrypt volume
 *
 */
class FormattingSubException : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FormattingSubException() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param subExceptionString Description of the exception occured while dismounting the volume
     */
    FormattingSubException(QString fonction, QString filename, quint32 line,
                           QString subExceptionString) : CoreException(fonction, filename, line),
        subExceptionString(subExceptionString) {}
    DEF_EXCEPTION_WHAT(FormattingSubException, CoreException,
                       "Exception occured during formatting:\n" + subExceptionString + "--------------\n")
 protected:
    QString subExceptionString; /**< Description of the exception occured while dismounting the volume */
    DEC_SERIALIZABLE(FormattingSubException);
};

#define FormatProcessFailedException() GostCrypt::Core::FormatProcessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when the process in charge of formatting the newly created volume fail
 *
 */
class FormatProcessFailed : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FormatProcessFailed() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    FormatProcessFailed(QString fonction, QString filename, quint32 line) : CoreException(fonction,
                filename,
                line) {}
    DEF_EXCEPTION_WHAT(FormatProcessFailed, CoreException,
                       "The executed process was shut down or failed unexpectedly.\n")
 protected:
    DEC_SERIALIZABLE(FormatProcessFailed);
};

#define FilesystemNotSupportedException(filesystem) GostCrypt::Core::FilesystemNotSupported(__PRETTY_FUNCTION__, __FILE__, __LINE__, filesystem);
/**
 * @brief Exception thrown when the tool to format the newly created volume with the requested filesystem type in not found
 *
 */
class FilesystemNotSupported : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FilesystemNotSupported() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param filesystem Name of the filesystem requested for the newly created volume
     */
    FilesystemNotSupported(QString fonction, QString filename, quint32 line,
                           QString filesystem) : CoreException(fonction, filename, line), filesystem(filesystem) {}
    DEF_EXCEPTION_WHAT(FilesystemNotSupported, CoreException,
                       "Filesystem not supported : " + filesystem + "\n")
 protected:
    QString filesystem; /**< Name of the filesystem requested for the newly created volume */
    DEC_SERIALIZABLE(FilesystemNotSupported);
};

#define AlgorithmNotFoundException(algorithm) GostCrypt::Core::AlgorithmNotFound(__PRETTY_FUNCTION__, __FILE__, __LINE__, algorithm);
/**
 * @brief Exception thrown when no algorithm corresponding to the requested algorithm name can be found
 */
class AlgorithmNotFound : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    AlgorithmNotFound() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param algorithm Name of the algorithm requested
     */
    AlgorithmNotFound(QString fonction, QString filename, quint32 line,
                      QString algorithm) : CoreException(fonction, filename, line), algorithm(algorithm) {}
    DEF_EXCEPTION_WHAT(AlgorithmNotFound, CoreException,
                       "Filesystem not supported : " + algorithm + "\n")
 protected:
    QString algorithm; /**< Name of the algorithm requested */
    DEC_SERIALIZABLE(AlgorithmNotFound);
};

#define RandomNumberGeneratorNotRunningException() GostCrypt::Core::RandomNumberGeneratorNotRunning(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when trying to use RandomGenerator before it had been started
 */
class RandomNumberGeneratorNotRunning : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    RandomNumberGeneratorNotRunning() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     */
    RandomNumberGeneratorNotRunning(QString fonction, QString filename,
                                    quint32 line) : CoreException(fonction, filename, line) {}
    DEF_EXCEPTION_WHAT(RandomNumberGeneratorNotRunning, CoreException,
                       "The random number generator is not running")
 protected:
    DEC_SERIALIZABLE(RandomNumberGeneratorNotRunning);
};

#define FailedUsingSystemRandomSourceException(errorNumber) GostCrypt::Core::FailedUsingSystemRandomSource(__PRETTY_FUNCTION__, __FILE__, __LINE__, errorNumber);
/**
 * @brief Exception thrown when the system call to get random bytes fail
 */
class FailedUsingSystemRandomSource : public CoreException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    FailedUsingSystemRandomSource() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function in which the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param errorNumber Value of the errno variable after the failed system call
     */
    FailedUsingSystemRandomSource(QString fonction, QString filename, quint32 line,
                                  qint32 errorNumber) : CoreException(fonction, filename, line), errorNumber(errorNumber) {}
    DEF_EXCEPTION_WHAT(FailedUsingSystemRandomSource, CoreException,
                       "Failed using system random source : " + QString(strerror(errorNumber)) + " (" + QString::number(
                           errorNumber) + ")")
 protected:
    qint32 errorNumber;
    DEC_SERIALIZABLE(FailedUsingSystemRandomSource);
};
//TODO check that all exceptions in these file should not be in another exception file
}
}

SERIALIZABLE(GostCrypt::Core::CoreException)
SERIALIZABLE(GostCrypt::Core::DeviceNotMounted)
SERIALIZABLE(GostCrypt::Core::VolumeAlreadyMounted)
SERIALIZABLE(GostCrypt::Core::FailedOpenVolume)
SERIALIZABLE(GostCrypt::Core::MountPointUsed)
SERIALIZABLE(GostCrypt::Core::MountFilesystemManagerException)
SERIALIZABLE(GostCrypt::Core::FailMountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailUnmountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailedAttachLoopDevice)
SERIALIZABLE(GostCrypt::Core::FailedDetachLoopDevice)
SERIALIZABLE(GostCrypt::Core::VolumeNotMounted)
SERIALIZABLE(GostCrypt::Core::FormattingSubException)
SERIALIZABLE(GostCrypt::Core::FormatProcessFailed)
SERIALIZABLE(GostCrypt::Core::FilesystemNotSupported)
SERIALIZABLE(GostCrypt::Core::AlgorithmNotFound)
SERIALIZABLE(GostCrypt::Core::IncorrectSudoPassword)
SERIALIZABLE(GostCrypt::Core::ServiceProcessCrashed)
SERIALIZABLE(GostCrypt::Core::FailFindFilesystemType)
SERIALIZABLE(GostCrypt::Core::RandomNumberGeneratorNotRunning)
SERIALIZABLE(GostCrypt::Core::FailedUsingSystemRandomSource)

#endif // COREEXCEPTION_H

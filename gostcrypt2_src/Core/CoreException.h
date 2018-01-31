#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include <QString>
#include <QVariant>
#include <QDebug>
#include "GostCryptException.h"

namespace GostCrypt {
	namespace Core {
        /**
         * @brief
         *
         */
        void initCoreException();

		class CoreException : public GostCryptException {
			public:
				CoreException() {}
				CoreException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
				DEF_EXCEPTION_WHAT(CoreException, GostCryptException, "")

			DEC_SERIALIZABLE(CoreException);
		};

		#define DeviceNotMountedException(devicePath) GostCrypt::Core::DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
        /**
         * @brief
         *
         */
        class DeviceNotMounted : public CoreException {
			public:
                /**
                 * @brief
                 *
                 */
                DeviceNotMounted() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param device
                 */
                DeviceNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> device) : CoreException(fonction, filename, line), device(device) {}
                DEF_EXCEPTION_WHAT(DeviceNotMounted, CoreException, "The device "+device->absoluteFilePath() + " is not mounted.\n")
			protected:
                QSharedPointer<QFileInfo> device; /**< TODO: describe */

			DEC_SERIALIZABLE(DeviceNotMounted);
		};

		#define MissingParamException(param) GostCrypt::Core::MissingParam(__PRETTY_FUNCTION__, __FILE__, __LINE__, param);
        /**
         * @brief
         *
         */
        class MissingParam : public CoreException {
			public:
                /**
                 * @brief
                 *
                 */
                MissingParam() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param param
                 */
                MissingParam(QString fonction, QString filename, quint32 line, QString param) : CoreException(fonction, filename, line), param(param) {}
                DEF_EXCEPTION_WHAT(MissingParam, CoreException, "The parameter " + param + " is mandatory and should be defined.\n")
			protected:
                QString param; /**< TODO: describe */
			DEC_SERIALIZABLE(MissingParam);
		};

		#define InvalidParamException(param) GostCrypt::Core::InvalidParam(__PRETTY_FUNCTION__, __FILE__, __LINE__, param);
        /**
         * @brief
         *
         */
        class InvalidParam : public CoreException {
			public:
                /**
                 * @brief
                 *
                 */
                InvalidParam() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param param
                 */
                InvalidParam(QString fonction, QString filename, quint32 line, QString param) : CoreException(fonction, filename, line), param(param) {}
                DEF_EXCEPTION_WHAT(InvalidParam, CoreException, "The parameter " + param + " is invalid.\n")
			protected:
                QString param; /**< TODO: describe */
			DEC_SERIALIZABLE(InvalidParam);
		};

		#define VolumeAlreadyMountedException(volumePath) GostCrypt::Core::VolumeAlreadyMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        /**
         * @brief
         *
         */
        class VolumeAlreadyMounted : public CoreException {
			public:
                /**
                 * @brief
                 *
                 */
                VolumeAlreadyMounted() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param volumePath
                 */
                VolumeAlreadyMounted(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, "The volume " + volumePath.absoluteFilePath() + " is already mounted.\n")
			protected:
                QFileInfo volumePath; /**< TODO: describe */
			DEC_SERIALIZABLE(VolumeAlreadyMounted);
		};

		#define FailedOpenVolumeException(volumePath) GostCrypt::Core::FailedOpenVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        /**
         * @brief
         *
         */
        class FailedOpenVolume : public CoreException {
			public:
                /**
                 * @brief
                 *
                 */
                FailedOpenVolume() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param volumePath
                 */
                FailedOpenVolume(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(FailedOpenVolume, CoreException, "Opening of volume " + volumePath.absoluteFilePath() + " failed.\n")
			protected:
                QFileInfo volumePath; /**< TODO: describe */
			DEC_SERIALIZABLE(FailedOpenVolume);
		};

        #define IncorrectSectorSizeException() GostCrypt::Core::IncorrectSectorSize(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        /**
         * @brief
         *
         */
        class IncorrectSectorSize : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                IncorrectSectorSize() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 */
                IncorrectSectorSize(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(IncorrectSectorSize, CoreException, "The sector size read from the header does not correspond to the device sector size or is unsupported.\n")
            protected:
            DEC_SERIALIZABLE(IncorrectSectorSize);
        };

        #define IncorrectSudoPasswordException() GostCrypt::Core::IncorrectSudoPassword(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        /**
         * @brief
         *
         */
        class IncorrectSudoPassword : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                IncorrectSudoPassword() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 */
                IncorrectSudoPassword(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(IncorrectSudoPassword, CoreException, "The given passwords are wrong, can't execute request.\n")
            protected:
            DEC_SERIALIZABLE(IncorrectSudoPassword);
        };

        #define WorkerProcessCrashedException() GostCrypt::Core::WorkerProcessCrashed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        /**
         * @brief
         *
         */
        class WorkerProcessCrashed : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                WorkerProcessCrashed() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 */
                WorkerProcessCrashed(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(WorkerProcessCrashed, CoreException, "The worker process crashed unexpectedly.\n")
            protected:
            DEC_SERIALIZABLE(WorkerProcessCrashed);
        };

        #define MountPointUsedException(mountpoint) GostCrypt::Core::MountPointUsed(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        /**
         * @brief
         *
         */
        class MountPointUsed : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                MountPointUsed() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param mountpoint
                 */
                MountPointUsed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : CoreException(fonction, filename, line), mountpoint(mountpoint) {}
                /**
                 * @brief
                 *
                 * @return QSharedPointer<QFileInfo>
                 */
                QSharedPointer<QFileInfo> getMountpoint() { return mountpoint; }
                DEF_EXCEPTION_WHAT(MountPointUsed, CoreException, "The directory " + mountpoint->absoluteFilePath() + " is already used\n")
            protected:
                QSharedPointer<QFileInfo> mountpoint; /**< TODO: describe */
            DEC_SERIALIZABLE(MountPointUsed);
        };

        /**
         * @brief
         *
         */
        class MountFilesystemManagerException : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                MountFilesystemManagerException() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param error_number
                 * @param mountpoint
                 */
                MountFilesystemManagerException(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : CoreException(fonction, filename, line), error_number(error_number), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(MountFilesystemManagerException, CoreException, "")
            protected:
                qint32 error_number; /**< TODO: describe */
                QSharedPointer<QFileInfo> mountpoint; /**< TODO: describe */

            DEC_SERIALIZABLE(MountFilesystemManagerException);
        };

        #define FailMountFilesystemException(error_number, mountpoint, devicePath, filesystemtype) GostCrypt::Core::FailMountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint, devicePath, filesystemtype);
        /**
         * @brief
         *
         */
        class FailMountFilesystem : public MountFilesystemManagerException {
            public:
                /**
                 * @brief
                 *
                 */
                FailMountFilesystem() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param error_number
                 * @param mountpoint
                 * @param devicePath
                 */
                FailMountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint, QSharedPointer<QFileInfo> devicePath, QString filesystemtype) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint), devicePath(devicePath), filesystemtype(filesystemtype) {}
                DEF_EXCEPTION_WHAT(FailMountFilesystem, MountFilesystemManagerException, "Unable to mount " + devicePath->absoluteFilePath() + " in " + mountpoint->absoluteFilePath() + " as " +filesystemtype+ " ("+QString::number(error_number) + ":" + QString(strerror(error_number)) + ")\n")
            protected:
                QSharedPointer<QFileInfo> devicePath; /**< TODO: describe */
                QString filesystemtype;

            DEC_SERIALIZABLE(FailMountFilesystem);
        };

        #define FailFindFilesystemTypeException(devicePath) GostCrypt::Core::FailFindFilesystemType(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
        /**
         * @brief
         *
         */
        class FailFindFilesystemType : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                FailFindFilesystemType() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param devicePath
                 */
                FailFindFilesystemType(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> devicePath) : CoreException(fonction, filename, line), devicePath(devicePath) {}
                DEF_EXCEPTION_WHAT(FailFindFilesystemType, CoreException, "Unable to mount " + devicePath->absoluteFilePath() + ": Failed to find filesystem type.")
            protected:
                QSharedPointer<QFileInfo> devicePath; /**< TODO: describe */

            DEC_SERIALIZABLE(FailFindFilesystemType);
        };

        #define FailUnmountFilesystemException(error_number, mountpoint) GostCrypt::Core::FailUnmountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint);
        /**
         * @brief
         *
         */
        class FailUnmountFilesystem : public MountFilesystemManagerException {
            public:
                /**
                 * @brief
                 *
                 */
                FailUnmountFilesystem() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param error_number
                 * @param mountpoint
                 */
                FailUnmountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint) {}
                DEF_EXCEPTION_WHAT(FailUnmountFilesystem, MountFilesystemManagerException, "Unable to unmount filesystem in " + mountpoint->absoluteFilePath() + "("+ QString(strerror(error_number)) + ")\n")
            DEC_SERIALIZABLE(FailUnmountFilesystem);
        };

		#define FailedAttachLoopDeviceException(imageFile) GostCrypt::Core::FailedAttachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, imageFile);
        /**
         * @brief
         *
         */
        class FailedAttachLoopDevice : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                FailedAttachLoopDevice() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param imageFile
                 */
                FailedAttachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> imageFile) : CoreException(fonction, filename, line), imageFile(imageFile) { }
                DEF_EXCEPTION_WHAT(FailedAttachLoopDevice, CoreException, QStringLiteral("Unable to create loop device for image file ") + imageFile->absoluteFilePath() + "\n")
            protected:
                QSharedPointer<QFileInfo> imageFile; /**< TODO: describe */

            DEC_SERIALIZABLE(FailedAttachLoopDevice);
        };

        #define FailedDetachLoopDeviceException(loopDevice) GostCrypt::Core::FailedDetachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, loopDevice);
        /**
         * @brief
         *
         */
        class FailedDetachLoopDevice : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                FailedDetachLoopDevice() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param loopDevice
                 */
                FailedDetachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> loopDevice) : CoreException(fonction, filename, line), loopDevice(loopDevice) { }
                DEF_EXCEPTION_WHAT(FailedDetachLoopDevice, CoreException, QStringLiteral("Unable to detach loop device ") + loopDevice->absoluteFilePath() + "\n")
            protected:
                QSharedPointer<QFileInfo> loopDevice; /**< TODO: describe */

            DEC_SERIALIZABLE(FailedDetachLoopDevice);
        };

        #define VolumeNotMountedException(volumePath) GostCrypt::Core::VolumeNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        /**
         * @brief
         *
         */
        class VolumeNotMounted : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                VolumeNotMounted() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param volumePath
                 */
                VolumeNotMounted(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeNotMounted, CoreException, "The volume " + volumePath.absoluteFilePath() + " is not mounted.\n")
            protected:
                QFileInfo volumePath; /**< TODO: describe */
            DEC_SERIALIZABLE(VolumeNotMounted);
        };

        #define ContentSizeInvalidException(size) GostCrypt::Core::ContentSizeInvalid(__PRETTY_FUNCTION__, __FILE__, __LINE__, size);
        /**
         * @brief
         *
         */
        class ContentSizeInvalid : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                ContentSizeInvalid() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param size
                 */
                ContentSizeInvalid(QString fonction, QString filename, quint32 line, qreal size) : CoreException(fonction, filename, line), size(size) {}
                DEF_EXCEPTION_WHAT(ContentSizeInvalid, CoreException, "Content size is invalid, must be between 0 and 1. Found : "+QString::number(size)+"\n")
            protected:
                qreal size; /**< TODO: describe */
            DEC_SERIALIZABLE(ContentSizeInvalid);
        };

        #define InvalidHeaderOffsetException(headersize, headeroffset) GostCrypt::Core::InvalidHeaderOffset(__PRETTY_FUNCTION__, __FILE__, __LINE__, headersize, headeroffset);
        /**
         * @brief
         *
         */
        class InvalidHeaderOffset : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                InvalidHeaderOffset() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param headeroffset
                 * @param headersize
                 */
                InvalidHeaderOffset(QString fonction, QString filename, quint32 line, qint32 headeroffset, quint32 headersize) : CoreException(fonction, filename, line), headeroffset(headeroffset), headersize(headersize) { }
                DEF_EXCEPTION_WHAT(InvalidHeaderOffset, CoreException, "Header size ("+ QString::number(headersize)+") not compatible with header offset ("+QString::number(headeroffset)+") ! This error comes from the Layout definition.\n")
            protected:
                qint32 headeroffset; /**< TODO: describe */
                quint32 headersize; /**< TODO: describe */
            DEC_SERIALIZABLE(InvalidHeaderOffset);
        };

        #define FormattingSubExceptionException(subExceptionString) GostCrypt::Core::FormattingSubException(__PRETTY_FUNCTION__, __FILE__, __LINE__, subExceptionString);
        /**
         * @brief
         *
         */
        class FormattingSubException : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                FormattingSubException() {}

FormattingSubException(QString fonction, QString filename, quint32 line, QString subExceptionString) : CoreException(fonction, filename, line), subExceptionString(subExceptionString){}
                DEF_EXCEPTION_WHAT(FormattingSubException, CoreException, "Exception occured during formatting:\n"+ subExceptionString +"--------------\n")
            protected:
                QString subExceptionString;
            DEC_SERIALIZABLE(FormattingSubException);
        };

        #define ProcessFailedException() GostCrypt::Core::ProcessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        /**
         * @brief
         *
         */
        class ProcessFailed : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                ProcessFailed() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 */
                ProcessFailed(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(ProcessFailed, CoreException, "The executed process was shut down or failed unexpectedly.\n")
            protected:
            DEC_SERIALIZABLE(ProcessFailed);
        };

        #define FilesystemNotSupportedException(filesystem) GostCrypt::Core::FilesystemNotSupported(__PRETTY_FUNCTION__, __FILE__, __LINE__, filesystem);
        /**
         * @brief
         *
         */
        class FilesystemNotSupported : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                FilesystemNotSupported() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param filesystem
                 */
                FilesystemNotSupported(QString fonction, QString filename, quint32 line, QString filesystem) : CoreException(fonction, filename, line), filesystem(filesystem) {}
                DEF_EXCEPTION_WHAT(FilesystemNotSupported, CoreException, "Filesystem not supported : "+filesystem+"\n")
            protected:
                QString filesystem; /**< TODO: describe */
            DEC_SERIALIZABLE(FilesystemNotSupported);
        };

        #define AlgorithmNotFoundException(algorithm) GostCrypt::Core::AlgorithmNotFound(__PRETTY_FUNCTION__, __FILE__, __LINE__, algorithm);
        class AlgorithmNotFound : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                AlgorithmNotFound() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param algorithm
                 */
                AlgorithmNotFound(QString fonction, QString filename, quint32 line, QString algorithm) : CoreException(fonction, filename, line), algorithm(algorithm) {}
                DEF_EXCEPTION_WHAT(AlgorithmNotFound, CoreException, "Filesystem not supported : "+algorithm+"\n")
            protected:
                QString algorithm; /**< TODO: describe */
            DEC_SERIALIZABLE(AlgorithmNotFound);
        };
        #define IncorrectVolumePasswordException(volumePath) GostCrypt::Core::IncorrectVolumePassword(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        class IncorrectVolumePassword : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                IncorrectVolumePassword() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param volumePath
                 */
                IncorrectVolumePassword(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(IncorrectVolumePassword, CoreException, "The given password for the volume " + volumePath->canonicalFilePath() + " is incorrect.\n")
            protected:
            QSharedPointer<QFileInfo> volumePath; /**< TODO: describe */
            DEC_SERIALIZABLE(IncorrectVolumePassword);
        };

        #define RandomNumberGeneratorNotRunningException() GostCrypt::Core::RandomNumberGeneratorNotRunning(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class RandomNumberGeneratorNotRunning : public CoreException {
            public:
                /**
                 * @brief Default Constructor used when deserializing
                 *
                 */
                RandomNumberGeneratorNotRunning() {}
                /**
                 * @brief Constructor used when throwing the exception
                 *
                 * @param fonction Name of the function where the exception is thrown
                 * @param filename Name of the file where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
                RandomNumberGeneratorNotRunning(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(RandomNumberGeneratorNotRunning, CoreException, "The random number generator is not running")
            protected:
            DEC_SERIALIZABLE(RandomNumberGeneratorNotRunning);
        };

        #define FailedUsingSystemRandomSourceException(errorCode) GostCrypt::Core::FailedUsingSystemRandomSource(__PRETTY_FUNCTION__, __FILE__, __LINE__, errorCode);
        class FailedUsingSystemRandomSource : public CoreException {
            public:
                /**
                 * @brief Default Constructor used when deserializing
                 *
                 */
                FailedUsingSystemRandomSource() {}
                /**
                 * @brief Constructor used when throwing the exception
                 *
                 * @param fonction Name of the function where the exception is thrown
                 * @param filename Name of the file where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
                FailedUsingSystemRandomSource(QString fonction, QString filename, quint32 line, qint32 errorCode) : CoreException(fonction, filename, line), errorCode(errorCode) {}
                DEF_EXCEPTION_WHAT(FailedUsingSystemRandomSource, CoreException, "Failed using system random source. ("+ QString::number(errorCode)+")")
            protected:
            qint32 errorCode;
            DEC_SERIALIZABLE(FailedUsingSystemRandomSource);
        };

        #define TestFailedException(testName) GostCrypt::Volume::TestFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__, testName);
        class TestFailed : public GostCryptException
        {
         public:
            TestFailed() {}
            /**
             * @brief Base class for all exception concerning Volume module
             *
             * @param fonction Name of the function where the exception was thrown
             * @param filename Path of the file where the exception was thrown
             * @param line Line of the file where the exception was thrown
             */
            TestFailed(QString fonction, QString filename, quint32 line, QString testName) : GostCryptException(fonction,
                        filename, line), testName(testName) {}
            DEF_EXCEPTION_WHAT(TestFailed, GostCryptException, "The following test failed: " + this->testName)
            QString testName;
            DEC_SERIALIZABLE(TestFailed);
        };

	}
}

SERIALIZABLE(GostCrypt::Core::CoreException)
SERIALIZABLE(GostCrypt::Core::DeviceNotMounted)
SERIALIZABLE(GostCrypt::Core::MissingParam)
SERIALIZABLE(GostCrypt::Core::VolumeAlreadyMounted)
SERIALIZABLE(GostCrypt::Core::FailedOpenVolume)
SERIALIZABLE(GostCrypt::Core::IncorrectSectorSize)
SERIALIZABLE(GostCrypt::Core::MountPointUsed)
SERIALIZABLE(GostCrypt::Core::MountFilesystemManagerException)
SERIALIZABLE(GostCrypt::Core::FailMountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailUnmountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailedAttachLoopDevice)
SERIALIZABLE(GostCrypt::Core::FailedDetachLoopDevice)
SERIALIZABLE(GostCrypt::Core::VolumeNotMounted)
SERIALIZABLE(GostCrypt::Core::ContentSizeInvalid)
SERIALIZABLE(GostCrypt::Core::InvalidHeaderOffset)
SERIALIZABLE(GostCrypt::Core::FormattingSubException)
SERIALIZABLE(GostCrypt::Core::ProcessFailed)
SERIALIZABLE(GostCrypt::Core::FilesystemNotSupported)
SERIALIZABLE(GostCrypt::Core::AlgorithmNotFound)
SERIALIZABLE(GostCrypt::Core::IncorrectSudoPassword)
SERIALIZABLE(GostCrypt::Core::WorkerProcessCrashed)
SERIALIZABLE(GostCrypt::Core::FailFindFilesystemType)
SERIALIZABLE(GostCrypt::Core::InvalidParam)
SERIALIZABLE(GostCrypt::Core::IncorrectVolumePassword)
SERIALIZABLE(GostCrypt::Core::RandomNumberGeneratorNotRunning)
SERIALIZABLE(GostCrypt::Core::FailedUsingSystemRandomSource)
SERIALIZABLE(GostCrypt::Core::TestFailed)


#endif // COREEXCEPTION_H

#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include <QString>
#include <QVariant>
#include <QDebug>
#include "SerializationUtil.h"

#define DEF_EXCEPTION_WHAT(exceptionName, parent, message) \
	virtual QString getName() const { \
		return parent::getName() + "/"#exceptionName; \
	} \
    virtual QString getMessage() const{ \
        return QString() + message; \
    } \
    virtual QString qwhat() const { \
        return parent::qwhat() + message; \
    } \
    virtual QVariant toQVariant() const { \
		return QVariant::fromValue(*this); \
	} \
	virtual void raise() const { \
		throw *this; \
	}

namespace GostCrypt {
	namespace Core {
        /**
         * @brief
         *
         */
        void initCoreException();

		class GostCryptException : public QException {
			public:
                GostCryptException() {}
                GostCryptException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
				quint32 getLine() const {return line; }
				QString getFilename() const {return filename; }
				QString getFonction() const {return fonction; }
				GostCryptException *clone() const { return new GostCryptException(*this); }
				const char * what () const throw () {
					return qwhat().toLocal8Bit().data();
				}
                /**
                 * @brief
                 *
                 * @return QString
                 */
                virtual QString qwhat() const {
                    return "\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" + getPosition() + "\nMessage:\t";
                }
                /**
                 * @brief fonction used to get the name of an exception
                 *
                 * @return QString return the name of the esception
                 */
                virtual QString getName() const {
                    return "";
                }
                /**
                 * @brief fonction used to get a message
                 *
                 * @return QString
                 */
                virtual QString getMessage() const{
                    return QString();
                }
                /**
                 * @brief
                 *
                 * @return QVariant
                 */
                virtual QVariant toQVariant() const {
					return QVariant::fromValue(*this);
				}
                /**
                 * @brief
                 *
                 */
                virtual void raise() const {
					throw *this;
				}
                /**
                 * @brief fonction to display the exception name as a message
                 *
                 * @return QString return the exception name
                 */
                QString displayedMessage() const {
#ifdef QT_DEBUG
					return qwhat();
#else
					return getMessage();
#endif
				}

			protected:
                /**
                 * @brief
                 *
                 * @return QString
                 */
                QString getPosition() const {
					return filename+":"+QString::number(line);
				}
                QString fonction;
				QString filename;
				quint32 line;
			DEC_SERIALIZABLE(GostCryptException);
		};

		class CoreException : public GostCryptException {
			public:
				CoreException() {}
				CoreException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
				DEF_EXCEPTION_WHAT(CoreException, GostCryptException, "")

			DEC_SERIALIZABLE(CoreException);
		};

		#define SystemExceptionException() SystemException(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        /**
         * @brief
         *
         */
        class SystemException : public CoreException {
			public:
				SystemException() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 */
                SystemException(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
				DEF_EXCEPTION_WHAT(SystemException, CoreException, "")

			DEC_SERIALIZABLE(SystemException);
		};



		#define FailedOpenFileException(file) GostCrypt::Core::FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
        /**
         * @brief
         *
         */
        class FailedOpenFile : public SystemException {
			public:
                /**
                 * @brief
                 *
                 */
                FailedOpenFile() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param file
                 */
                FailedOpenFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
                DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException, "Unable to open file \""+file.absoluteFilePath() + "\".\n")
            protected:
                QFileInfo file; /**< TODO: describe */
			DEC_SERIALIZABLE(FailedOpenFile);
		};

        #define FailedCreateDirectoryException(dir) GostCrypt::Core::FailedCreateDirectory(__PRETTY_FUNCTION__, __FILE__, __LINE__, dir);
        /**
         * @brief
         *
         */
        class FailedCreateDirectory : public SystemException {
            public:
                /**
                 * @brief
                 *
                 */
                FailedCreateDirectory() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param dir
                 */
                FailedCreateDirectory(QString fonction, QString filename, quint32 line, QString dir) : SystemException(fonction, filename, line), dir(dir) {}
                DEF_EXCEPTION_WHAT(FailedCreateDirectory, SystemException, "Fail to create directory \"" + dir + "\".\n")
            protected:
                QString dir; /**< TODO: describe */
            DEC_SERIALIZABLE(FailedCreateDirectory);
        };

		#define DeviceNotMountedException(devicePath) GostCrypt::Core::DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
        /**
         * @brief
         *
         */
        class DeviceNotMounted : public SystemException {
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
                DeviceNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> device) : SystemException(fonction, filename, line), device(device) {}
                DEF_EXCEPTION_WHAT(DeviceNotMounted, SystemException, "The device "+device->absoluteFilePath() + " is not mounted.\n")
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
                DEF_EXCEPTION_WHAT(MissingParam, CoreException, "The parameter " + param + " is invalid.\n")
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
                VolumeAlreadyMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, "The volume " + volumePath->absoluteFilePath() + " is already mounted.\n")
			protected:
                QSharedPointer<QFileInfo> volumePath; /**< TODO: describe */
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
                FailedOpenVolume(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(FailedOpenVolume, CoreException, "Opening of volume " + volumePath->absoluteFilePath() + " failed.\n")
			protected:
                QSharedPointer<QFileInfo> volumePath; /**< TODO: describe */
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
        class MountPointUsed : public SystemException {
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
                MountPointUsed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
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

        #define FailedCreateFuseMountPointException(mountpoint) GostCrypt::Core::FailedCreateFuseMountPoint(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        /**
         * @brief
         *
         */
        class FailedCreateFuseMountPoint : public SystemException {
            public:
                /**
                 * @brief
                 *
                 */
                FailedCreateFuseMountPoint() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param mountpoint
                 */
                FailedCreateFuseMountPoint(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(FailedCreateFuseMountPoint, CoreException, "Creation of fuse mount point " + mountpoint->absoluteFilePath() + " failed\n")
            protected:
                QSharedPointer<QFileInfo> mountpoint; /**< TODO: describe */
            DEC_SERIALIZABLE(FailedCreateFuseMountPoint);
        };

        /**
         * @brief
         *
         */
        class MountFilesystemManagerException : public SystemException {
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
                MountFilesystemManagerException(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), error_number(error_number), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(MountFilesystemManagerException, CoreException, "")
            protected:
                qint32 error_number; /**< TODO: describe */
                QSharedPointer<QFileInfo> mountpoint; /**< TODO: describe */

            DEC_SERIALIZABLE(MountFilesystemManagerException);
        };

		#define FailMountFilesystemException(error_number, mountpoint, devicePath) GostCrypt::Core::FailMountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint, devicePath);
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
                FailMountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint, QSharedPointer<QFileInfo> devicePath) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint), devicePath(devicePath) {}
                DEF_EXCEPTION_WHAT(FailMountFilesystem, MountFilesystemManagerException, "Unable to mount " + devicePath->absoluteFilePath() + " in " + mountpoint->absoluteFilePath() + "("+ QString::number(error_number) + ": " + QString(strerror(error_number)) + ")\n")
            protected:
                QSharedPointer<QFileInfo> devicePath; /**< TODO: describe */

            DEC_SERIALIZABLE(FailMountFilesystem);
        };

        #define FailFindFilesystemTypeException(devicePath) GostCrypt::Core::FailFindFilesystemType(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
        /**
         * @brief
         *
         */
        class FailFindFilesystemType : public SystemException {
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
                FailFindFilesystemType(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> devicePath) : SystemException(fonction, filename, line), devicePath(devicePath) {}
                DEF_EXCEPTION_WHAT(FailFindFilesystemType, SystemException, "Unable to mount " + devicePath->absoluteFilePath() + ": Failed to find filesystem type.")
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
        class FailedAttachLoopDevice : public SystemException {
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
                FailedAttachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> imageFile) : SystemException(fonction, filename, line), imageFile(imageFile) { }
                DEF_EXCEPTION_WHAT(FailedAttachLoopDevice, SystemException, QStringLiteral("Unable to create loop device for image file ") + imageFile->absoluteFilePath() + "\n")
            protected:
                QSharedPointer<QFileInfo> imageFile; /**< TODO: describe */

            DEC_SERIALIZABLE(FailedAttachLoopDevice);
        };

        #define FailedDetachLoopDeviceException(loopDevice) GostCrypt::Core::FailedDetachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, loopDevice);
        /**
         * @brief
         *
         */
        class FailedDetachLoopDevice : public SystemException {
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
                FailedDetachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> loopDevice) : SystemException(fonction, filename, line), loopDevice(loopDevice) { }
                DEF_EXCEPTION_WHAT(FailedDetachLoopDevice, SystemException, QStringLiteral("Unable to detach loop device ") + loopDevice->absoluteFilePath() + "\n")
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
                VolumeNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, "The volume " + volumePath->absoluteFilePath() + " is not mounted.\n")
            protected:
                QSharedPointer<QFileInfo> volumePath; /**< TODO: describe */
            DEC_SERIALIZABLE(VolumeNotMounted);
        };

        #define ExceptionFromVolumeException(message) GostCrypt::Core::ExceptionFromVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, message);
        /**
         * @brief
         *
         */
        class ExceptionFromVolume : public CoreException {
            public:
                /**
                 * @brief
                 *
                 */
                ExceptionFromVolume() {}
                /**
                 * @brief
                 *
                 * @param fonction
                 * @param filename
                 * @param line
                 * @param message
                 */
                ExceptionFromVolume(QString fonction, QString filename, quint32 line, QString message) : CoreException(fonction, filename, line), message(message) {}
                DEF_EXCEPTION_WHAT(ExceptionFromVolume, CoreException, message)
            protected:
                QString message; /**< TODO: describe */
            DEC_SERIALIZABLE(ExceptionFromVolume);
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

	}
}

SERIALIZABLE(GostCrypt::Core::GostCryptException)
SERIALIZABLE(GostCrypt::Core::CoreException)
SERIALIZABLE(GostCrypt::Core::SystemException)
SERIALIZABLE(GostCrypt::Core::FailedOpenFile)
SERIALIZABLE(GostCrypt::Core::DeviceNotMounted)
SERIALIZABLE(GostCrypt::Core::MissingParam)
SERIALIZABLE(GostCrypt::Core::VolumeAlreadyMounted)
SERIALIZABLE(GostCrypt::Core::FailedOpenVolume)
SERIALIZABLE(GostCrypt::Core::IncorrectSectorSize)
SERIALIZABLE(GostCrypt::Core::MountPointUsed)
SERIALIZABLE(GostCrypt::Core::FailedCreateFuseMountPoint)
SERIALIZABLE(GostCrypt::Core::MountFilesystemManagerException)
SERIALIZABLE(GostCrypt::Core::FailMountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailUnmountFilesystem)
SERIALIZABLE(GostCrypt::Core::FailedAttachLoopDevice)
SERIALIZABLE(GostCrypt::Core::FailedCreateDirectory)
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
SERIALIZABLE(GostCrypt::Core::ExceptionFromVolume)
SERIALIZABLE(GostCrypt::Core::FailFindFilesystemType)
SERIALIZABLE(GostCrypt::Core::InvalidParam)
SERIALIZABLE(GostCrypt::Core::IncorrectVolumePassword)

#endif // COREEXCEPTION_H

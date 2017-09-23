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
        return parent::qwhat() + getMessage(); \
    } \
    virtual QVariant toQVariant() const { \
		return QVariant::fromValue(*this); \
	} \
	virtual void raise() const { \
		throw *this; \
	}

namespace GostCrypt {
	namespace NewCore {
        void initCoreException();

		class CoreException : public QException {
			public:
                CoreException() {}
                CoreException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
				quint32 getLine() const {return line; }
				QString getFilename() const {return filename; }
				QString getFonction() const {return fonction; }
				CoreException *clone() const { return new CoreException(*this); }
				const char * what () const throw () {
					return qwhat().toLocal8Bit().data();
				}
                virtual QString qwhat() const {
                    return "\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" + getPosition() + "\nMessage:\t";
                }
                virtual QString getName() const {
                    return "/CoreException";
                }
                virtual QString getMessage() const{
                    return QString();
                }
                virtual QVariant toQVariant() const {
					return QVariant::fromValue(*this);
				}
				virtual void raise() const {
					throw *this;
				}
				QString displayedMessage() {
#ifdef QT_DEBUG
					return qwhat();
#else
					return getMessage();
#endif
				}

			protected:
				QString getPosition() const {
					return filename+":"+QString::number(line);
				}
                QString fonction;
				QString filename;
				quint32 line;
			DEC_SERIALIZABLE(CoreException);
		};

		#define SystemExceptionException() SystemException(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		class SystemException : public CoreException {
			public:
				SystemException() {}
				SystemException(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
				DEF_EXCEPTION_WHAT(SystemException, CoreException, "")

			DEC_SERIALIZABLE(SystemException);
		};



		#define FailedOpenFileException(file) FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
		class FailedOpenFile : public SystemException {
			public:
                FailedOpenFile() {}
                FailedOpenFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
                DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException, "Unable to open file \""+file.absoluteFilePath() + "\".\n")
            protected:
				QFileInfo file;
			DEC_SERIALIZABLE(FailedOpenFile);
		};

        #define FailedCreateDirectoryException(dir) FailedCreateDirectory(__PRETTY_FUNCTION__, __FILE__, __LINE__, dir);
        class FailedCreateDirectory : public SystemException {
            public:
                FailedCreateDirectory() {}
                FailedCreateDirectory(QString fonction, QString filename, quint32 line, QString dir) : SystemException(fonction, filename, line), dir(dir) {}
                DEF_EXCEPTION_WHAT(FailedCreateDirectory, SystemException, "Fail to create directory \"" + dir + "\".\n")
            protected:
                QString dir;
            DEC_SERIALIZABLE(FailedCreateDirectory);
        };

		#define DeviceNotMountedException(devicePath) DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
		class DeviceNotMounted : public SystemException {
			public:
                DeviceNotMounted() {}
                DeviceNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> device) : SystemException(fonction, filename, line), device(device) {}
                DEF_EXCEPTION_WHAT(DeviceNotMounted, SystemException, "The device "+device->absoluteFilePath() + " is not mounted.\n")
			protected:
                QSharedPointer<QFileInfo> device;

			DEC_SERIALIZABLE(DeviceNotMounted);
		};

		#define MissingParamException(param) MissingParam(__PRETTY_FUNCTION__, __FILE__, __LINE__, param);
		class MissingParam : public CoreException {
			public:
                MissingParam() {}
                MissingParam(QString fonction, QString filename, quint32 line, QString param) : CoreException(fonction, filename, line), param(param) {}
                DEF_EXCEPTION_WHAT(MissingParam, CoreException, "The parameter " + param + " is mandatory and should be defined.\n")
			protected:
				QString param;
			DEC_SERIALIZABLE(MissingParam);
		};

		#define InvalidParamException(param) InvalidParam(__PRETTY_FUNCTION__, __FILE__, __LINE__, param);
		class InvalidParam : public CoreException {
			public:
                InvalidParam() {}
                InvalidParam(QString fonction, QString filename, quint32 line, QString param) : CoreException(fonction, filename, line), param(param) {}
                DEF_EXCEPTION_WHAT(MissingParam, CoreException, "The parameter " + param + " is invalid.\n")
			protected:
				QString param;
			DEC_SERIALIZABLE(InvalidParam);
		};

		#define VolumeAlreadyMountedException(volumePath) VolumeAlreadyMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		class VolumeAlreadyMounted : public CoreException {
			public:
                VolumeAlreadyMounted() {}
                VolumeAlreadyMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, "The volume " + volumePath->absoluteFilePath() + " is already mounted.\n")
			protected:
				QSharedPointer<QFileInfo> volumePath;
			DEC_SERIALIZABLE(VolumeAlreadyMounted);
		};

		#define FailedOpenVolumeException(volumePath) FailedOpenVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		class FailedOpenVolume : public CoreException {
			public:
                FailedOpenVolume() {}
                FailedOpenVolume(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(FailedOpenVolume, CoreException, "Opening of volume " + volumePath->absoluteFilePath() + " failed.\n")
			protected:
                QSharedPointer<QFileInfo> volumePath;
			DEC_SERIALIZABLE(FailedOpenVolume);
		};

        #define IncorrectSectorSizeException() IncorrectSectorSize(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class IncorrectSectorSize : public CoreException {
            public:
                IncorrectSectorSize() {}
                IncorrectSectorSize(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(IncorrectSectorSize, CoreException, "The sector size read from the header does not correspond to the device sector size or is unsupported.\n")
            protected:
            DEC_SERIALIZABLE(IncorrectSectorSize);
        };

        #define IncorrectSudoPasswordException() IncorrectSudoPassword(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class IncorrectSudoPassword : public CoreException {
            public:
                IncorrectSudoPassword() {}
                IncorrectSudoPassword(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(IncorrectSudoPassword, CoreException, "The given passwords are wrong, can't execute request.\n")
            protected:
            DEC_SERIALIZABLE(IncorrectSudoPassword);
        };

        #define WorkerProcessCrashedException() WorkerProcessCrashed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class WorkerProcessCrashed : public CoreException {
            public:
                WorkerProcessCrashed() {}
                WorkerProcessCrashed(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(WorkerProcessCrashed, CoreException, "The worker process crashed unexpectedly.\n")
            protected:
            DEC_SERIALIZABLE(WorkerProcessCrashed);
        };

        #define MountPointUsedException(mountpoint) MountPointUsed(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        class MountPointUsed : public SystemException {
            public:
                MountPointUsed() {}
                MountPointUsed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
				QSharedPointer<QFileInfo> getMountpoint() { return mountpoint; }
                DEF_EXCEPTION_WHAT(MountPointUsed, CoreException, "The directory " + mountpoint->absoluteFilePath() + " is already used\n")
            protected:
                QSharedPointer<QFileInfo> mountpoint;
            DEC_SERIALIZABLE(MountPointUsed);
        };

        #define FailedCreateFuseMountPointException(mountpoint) FailedCreateFuseMountPoint(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        class FailedCreateFuseMountPoint : public SystemException {
            public:
                FailedCreateFuseMountPoint() {}
                FailedCreateFuseMountPoint(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(FailedCreateFuseMountPoint, CoreException, "Creation of fuse mount point " + mountpoint->absoluteFilePath() + " failed\n")
            protected:
                QSharedPointer<QFileInfo> mountpoint;
            DEC_SERIALIZABLE(FailedCreateFuseMountPoint);
        };

        class MountFilesystemManagerException : public SystemException {
            public:
                MountFilesystemManagerException() {}
                MountFilesystemManagerException(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), error_number(error_number), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(MountFilesystemManagerException, CoreException, "")
            protected:
                qint32 error_number;
                QSharedPointer<QFileInfo> mountpoint;

            DEC_SERIALIZABLE(MountFilesystemManagerException);
        };

		#define FailMountFilesystemException(error_number, mountpoint, devicePath) FailMountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint, devicePath);
        class FailMountFilesystem : public MountFilesystemManagerException {
            public:
                FailMountFilesystem() {}
                FailMountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint, QSharedPointer<QFileInfo> devicePath) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint), devicePath(devicePath) {}
                DEF_EXCEPTION_WHAT(FailMountFilesystem, MountFilesystemManagerException, "Unable to mount " + devicePath->absoluteFilePath() + " in " + mountpoint->absoluteFilePath() + "("+ QString::number(error_number) + ": " + QString(strerror(error_number)) + ")\n")
            protected:
                QSharedPointer<QFileInfo> devicePath;

            DEC_SERIALIZABLE(FailMountFilesystem);
        };

        #define FailFindFilesystemTypeException(devicePath) FailFindFilesystemType(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
        class FailFindFilesystemType : public SystemException {
            public:
                FailFindFilesystemType() {}
                FailFindFilesystemType(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> devicePath) : SystemException(fonction, filename, line), devicePath(devicePath) {}
                DEF_EXCEPTION_WHAT(FailFindFilesystemType, SystemException, "Unable to mount " + devicePath->absoluteFilePath() + ": Failed to find filesystem type.")
            protected:
                QSharedPointer<QFileInfo> devicePath;

            DEC_SERIALIZABLE(FailFindFilesystemType);
        };

        #define FailUnmountFilesystemException(error_number, mountpoint) FailUnmountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint);
        class FailUnmountFilesystem : public MountFilesystemManagerException {
            public:
                FailUnmountFilesystem() {}
                FailUnmountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint) {}
                DEF_EXCEPTION_WHAT(FailUnmountFilesystem, MountFilesystemManagerException, "Unable to unmount filesystem in " + mountpoint->absoluteFilePath() + "("+ QString(strerror(error_number)) + ")\n")
            DEC_SERIALIZABLE(FailUnmountFilesystem);
        };

		#define FailedAttachLoopDeviceException(imageFile) FailedAttachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, imageFile);
        class FailedAttachLoopDevice : public SystemException {
            public:
                FailedAttachLoopDevice() {}
                FailedAttachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> imageFile) : SystemException(fonction, filename, line), imageFile(imageFile) { }
                DEF_EXCEPTION_WHAT(FailedAttachLoopDevice, SystemException, QStringLiteral("Unable to create loop device for image file ") + imageFile->absoluteFilePath() + "\n")
            protected:
                QSharedPointer<QFileInfo> imageFile;

            DEC_SERIALIZABLE(FailedAttachLoopDevice);
        };

        #define FailedDetachLoopDeviceException(loopDevice) FailedDetachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, loopDevice);
        class FailedDetachLoopDevice : public SystemException {
            public:
                FailedDetachLoopDevice() {}
                FailedDetachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> loopDevice) : SystemException(fonction, filename, line), loopDevice(loopDevice) { }
                DEF_EXCEPTION_WHAT(FailedDetachLoopDevice, SystemException, QStringLiteral("Unable to detach loop device ") + loopDevice->absoluteFilePath() + "\n")
            protected:
                QSharedPointer<QFileInfo> loopDevice;

            DEC_SERIALIZABLE(FailedDetachLoopDevice);
        };

        #define VolumeNotMountedException(volumePath) VolumeNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        class VolumeNotMounted : public CoreException {
            public:
                VolumeNotMounted() {}
                VolumeNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, "The volume " + volumePath->absoluteFilePath() + " is not mounted.\n")
            protected:
                QSharedPointer<QFileInfo> volumePath;
            DEC_SERIALIZABLE(VolumeNotMounted);
        };

        #define ExceptionFromVolumeException(message) ExceptionFromVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, message);
        class ExceptionFromVolume : public CoreException {
            public:
                ExceptionFromVolume() {}
                ExceptionFromVolume(QString fonction, QString filename, quint32 line, QString message) : CoreException(fonction, filename, line), message(message) {}
                DEF_EXCEPTION_WHAT(ExceptionFromVolume, CoreException, message)
            protected:
                QString message;
            DEC_SERIALIZABLE(ExceptionFromVolume);
        };

        #define ContentSizeInvalidException(size) ContentSizeInvalid(__PRETTY_FUNCTION__, __FILE__, __LINE__, size);
        class ContentSizeInvalid : public CoreException {
            public:
                ContentSizeInvalid() {}
                ContentSizeInvalid(QString fonction, QString filename, quint32 line, qreal size) : CoreException(fonction, filename, line), size(size) {}
                DEF_EXCEPTION_WHAT(ContentSizeInvalid, CoreException, "Content size is invalid, must be between 0 and 1. Found : "+size+"\n")
            protected:
                qreal size;
            DEC_SERIALIZABLE(ContentSizeInvalid);
        };

        #define InvalidHeaderOffsetException(headersize, headeroffset) InvalidHeaderOffset(__PRETTY_FUNCTION__, __FILE__, __LINE__, headersize, headeroffset);
        class InvalidHeaderOffset : public CoreException {
            public:
                InvalidHeaderOffset() {}
                InvalidHeaderOffset(QString fonction, QString filename, quint32 line, qint32 headeroffset, quint32 headersize) : CoreException(fonction, filename, line), headeroffset(headeroffset), headersize(headersize) { }
                DEF_EXCEPTION_WHAT(InvalidHeaderOffset, CoreException, "Header size ("+headersize+") not compatible with header offset ("+headeroffset+") ! This error comes from the Layout definition.\n")
            protected:
                qint32 headeroffset;
                quint32 headersize;
            DEC_SERIALIZABLE(InvalidHeaderOffset);
        };

        #define FormattingSubExceptionException(exception) FormattingSubException(__PRETTY_FUNCTION__, __FILE__, __LINE__, exception);
        class FormattingSubException : public CoreException {
            public:
                FormattingSubException() {}
                FormattingSubException(QString fonction, QString filename, quint32 line, CoreException e) : CoreException(fonction, filename, line), e(e) {}
                DEF_EXCEPTION_WHAT(FormattingSubException, CoreException, "Exception occured during formatting:\n"+e.qwhat()+"--------------\n")
            protected:
                CoreException e;
            DEC_SERIALIZABLE(FormattingSubException);
        };

        #define ProcessFailedException() ProcessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class ProcessFailed : public CoreException {
            public:
                ProcessFailed() {}
                ProcessFailed(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(ProcessFailed, CoreException, "The executed process was shut down or failed unexpectedly.\n")
            protected:
            DEC_SERIALIZABLE(ProcessFailed);
        };

        #define FilesystemNotSupportedException(filesystem) FilesystemNotSupported(__PRETTY_FUNCTION__, __FILE__, __LINE__, filesystem);
        class FilesystemNotSupported : public CoreException {
            public:
                FilesystemNotSupported() {}
                FilesystemNotSupported(QString fonction, QString filename, quint32 line, QString filesystem) : CoreException(fonction, filename, line), filesystem(filesystem) {}
                DEF_EXCEPTION_WHAT(FilesystemNotSupported, CoreException, "Filesystem not supported : "+filesystem+"\n")
            protected:
                QString filesystem;
            DEC_SERIALIZABLE(FilesystemNotSupported);
        };

        #define AlgorithmNotFoundException(algorithm) FilesystemNotSupported(__PRETTY_FUNCTION__, __FILE__, __LINE__, algorithm);
        class AlgorithmNotFound : public CoreException {
            public:
                AlgorithmNotFound() {}
                AlgorithmNotFound(QString fonction, QString filename, quint32 line, QString algorithm) : CoreException(fonction, filename, line), algorithm(algorithm) {}
                DEF_EXCEPTION_WHAT(AlgorithmNotFound, CoreException, "Filesystem not supported : "+algorithm+"\n")
            protected:
                QString algorithm;
            DEC_SERIALIZABLE(AlgorithmNotFound);
        };

        #define UnknowRequestException(requestTypeName) UnknowRequest(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowRequest : public CoreException {
            public:
                UnknowRequest() {}
                UnknowRequest(QString fonction, QString filename, quint32 line, const char *requestTypeName) : CoreException(fonction, filename, line), requestTypeName(requestTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowRequest, CoreException, "Core received an unknown request (" + requestTypeName + ")")
            protected:
            QString requestTypeName;
            DEC_SERIALIZABLE(UnknowRequest);
        };

        #define UnknowResponseException(requestTypeName) UnknowResponse(__PRETTY_FUNCTION__, __FILE__, __LINE__, requestTypeName);
        class UnknowResponse : public CoreException {
            public:
                UnknowResponse() {}
                UnknowResponse(QString fonction, QString filename, quint32 line, const char *responseTypeName) : CoreException(fonction, filename, line), responseTypeName(responseTypeName) {}
                DEF_EXCEPTION_WHAT(UnknowResponse, CoreException, "Unknow reponse received from workr process (" + responseTypeName + ")")
            protected:
            QString responseTypeName;
            DEC_SERIALIZABLE(UnknowResponse);
        };

        #define IncorrectVolumePasswordException(volumePath) IncorrectVolumePassword(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
        class IncorrectVolumePassword : public CoreException {
            public:
                IncorrectVolumePassword() {}
                IncorrectVolumePassword(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(IncorrectVolumePassword, CoreException, "The given password for the volume " + volumePath->canonicalFilePath() + " is incorrect.")
            protected:
            QSharedPointer<QFileInfo> volumePath;
            DEC_SERIALIZABLE(IncorrectVolumePassword);
        };

	}
}

SERIALIZABLE(GostCrypt::NewCore::CoreException)
SERIALIZABLE(GostCrypt::NewCore::SystemException)
SERIALIZABLE(GostCrypt::NewCore::FailedOpenFile)
SERIALIZABLE(GostCrypt::NewCore::DeviceNotMounted)
SERIALIZABLE(GostCrypt::NewCore::MissingParam)
SERIALIZABLE(GostCrypt::NewCore::VolumeAlreadyMounted)
SERIALIZABLE(GostCrypt::NewCore::FailedOpenVolume)
SERIALIZABLE(GostCrypt::NewCore::IncorrectSectorSize)
SERIALIZABLE(GostCrypt::NewCore::MountPointUsed)
SERIALIZABLE(GostCrypt::NewCore::FailedCreateFuseMountPoint)
SERIALIZABLE(GostCrypt::NewCore::MountFilesystemManagerException)
SERIALIZABLE(GostCrypt::NewCore::FailMountFilesystem)
SERIALIZABLE(GostCrypt::NewCore::FailUnmountFilesystem)
SERIALIZABLE(GostCrypt::NewCore::FailedAttachLoopDevice)
SERIALIZABLE(GostCrypt::NewCore::FailedCreateDirectory)
SERIALIZABLE(GostCrypt::NewCore::FailedDetachLoopDevice)
SERIALIZABLE(GostCrypt::NewCore::VolumeNotMounted)
SERIALIZABLE(GostCrypt::NewCore::ContentSizeInvalid)
SERIALIZABLE(GostCrypt::NewCore::InvalidHeaderOffset)
SERIALIZABLE(GostCrypt::NewCore::FormattingSubException)
SERIALIZABLE(GostCrypt::NewCore::ProcessFailed)
SERIALIZABLE(GostCrypt::NewCore::FilesystemNotSupported)
SERIALIZABLE(GostCrypt::NewCore::AlgorithmNotFound)
SERIALIZABLE(GostCrypt::NewCore::IncorrectSudoPassword)
SERIALIZABLE(GostCrypt::NewCore::WorkerProcessCrashed)
SERIALIZABLE(GostCrypt::NewCore::ExceptionFromVolume)
SERIALIZABLE(GostCrypt::NewCore::UnknowRequest)
SERIALIZABLE(GostCrypt::NewCore::UnknowResponse)
SERIALIZABLE(GostCrypt::NewCore::FailFindFilesystemType)
SERIALIZABLE(GostCrypt::NewCore::InvalidParam)
SERIALIZABLE(GostCrypt::NewCore::IncorrectVolumePassword)

#endif // COREEXCEPTION_H

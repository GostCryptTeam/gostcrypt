#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include <QString>
#include <QVariant>
#include "SerializationUtil.h"

#define DEF_EXCEPTION_WHAT(exceptionName, parent, message) \
	virtual QString getName() const { \
		return parent::getName() + "/"#exceptionName; \
	} \
	virtual QString qwhat() const { \
		return parent::qwhat() + message; \
	}

namespace GostCrypt {
	namespace NewCore {
        bool initCoreException();

		class CoreException : public QException {
			public:
                CoreException() {}
                CoreException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
				quint32 getLine() const {return line; }
				QString getFilename() const {return filename; }
				QString getFonction() const {return fonction; }
				void raise() const { throw *this; }
				CoreException *clone() const { return new CoreException(*this); }
				const char * what () const throw () {
					return qwhat().toLocal8Bit().data();
				}
			protected:
				virtual QString qwhat() const {
					return "\n\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" + getPosition() + "\nMessage:\t";
				}
				QString getPosition() const {
					return filename+":"+QString::number(line);
				}
				virtual QString getName() const {
					return "/CoreException";
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
			protected:
				DEF_EXCEPTION_WHAT(SystemException, CoreException, "")

			DEC_SERIALIZABLE(SystemException);
		};



		#define FailedOpenFileException(file) FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
		class FailedOpenFile : public SystemException {
			public:
                FailedOpenFile() {}
                FailedOpenFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
			protected:
				QFileInfo file;
				DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException, " Unable to open file \""+file.canonicalFilePath() + "\".")


			DEC_SERIALIZABLE(FailedOpenFile);
		};

		#define DeviceNotMountedException(devicePath) DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
		class DeviceNotMounted : public SystemException {
			public:
                DeviceNotMounted() {}
                DeviceNotMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> device) : SystemException(fonction, filename, line), device(device) {}
			protected:
                QSharedPointer<QFileInfo> device;

                DEF_EXCEPTION_WHAT(DeviceNotMounted, SystemException, " The device "+device->canonicalFilePath() + " is not mounted.")
			DEC_SERIALIZABLE(DeviceNotMounted);
		};

		#define MissingParamException(param) MissingParam(__PRETTY_FUNCTION__, __FILE__, __LINE__, param);
		class MissingParam : public CoreException {
			public:
                MissingParam() {}
                MissingParam(QString fonction, QString filename, quint32 line, QString param) : CoreException(fonction, filename, line), param(param) {}
			protected:
				QString param;
				DEF_EXCEPTION_WHAT(MissingParam, CoreException, " The parameter " + param + " is mandatory and should be defined.")
			DEC_SERIALIZABLE(MissingParam);
		};

		#define VolumeAlreadyMountedException(volumePath) VolumeAlreadyMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		class VolumeAlreadyMounted : public CoreException {
			public:
                VolumeAlreadyMounted() {}
                VolumeAlreadyMounted(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
			protected:
				QSharedPointer<QFileInfo> volumePath;
				DEF_EXCEPTION_WHAT(VolumeAlreadyMounted, CoreException, " The volume " + volumePath->canonicalFilePath() + " is already mounted.")
			DEC_SERIALIZABLE(VolumeAlreadyMounted);
		};

		#define FailedOpenVolumeException(volumePath) FailedOpenVolume(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		class FailedOpenVolume : public CoreException {
			public:
                FailedOpenVolume() {}
                FailedOpenVolume(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : CoreException(fonction, filename, line), volumePath(volumePath) {}
			protected:
                QSharedPointer<QFileInfo> volumePath;
                DEF_EXCEPTION_WHAT(FailedOpenVolume, CoreException, " Opening of volume " + volumePath->canonicalFilePath() + " failed.")
			DEC_SERIALIZABLE(FailedOpenVolume);
		};

        #define IncorrectSectorSizeException() IncorrectSectorSize(__PRETTY_FUNCTION__, __FILE__, __LINE__);
        class IncorrectSectorSize : public CoreException {
            public:
                IncorrectSectorSize() {}
                IncorrectSectorSize(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
            protected:
                DEF_EXCEPTION_WHAT(IncorrectSectorSize, CoreException, "The sector size read from the header does not correspond to the device sector size or is unsupported.")
            DEC_SERIALIZABLE(IncorrectSectorSize);
        };

        #define MountPointUsedException(mountpoint) MountPointUsed(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        class MountPointUsed : public SystemException {
            public:
                MountPointUsed() {}
                MountPointUsed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
				QSharedPointer<QFileInfo> getMountpoint() { return mountpoint; }
            protected:
                QSharedPointer<QFileInfo> mountpoint;
                DEF_EXCEPTION_WHAT(MountPointUsed, CoreException, "The directory " + mountpoint->canonicalFilePath() + " is already used")
            DEC_SERIALIZABLE(MountPointUsed);
        };

        #define FailedCreateFuseMountPointException(mountpoint) FailedCreateFuseMountPoint(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        class FailedCreateFuseMountPoint : public SystemException {
            public:
                FailedCreateFuseMountPoint() {}
                FailedCreateFuseMountPoint(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), mountpoint(mountpoint) {}
            protected:
                QSharedPointer<QFileInfo> mountpoint;
                DEF_EXCEPTION_WHAT(FailedCreateFuseMountPoint, CoreException, "Creation of fuse mount point " + mountpoint->canonicalFilePath() + " failed")
            DEC_SERIALIZABLE(FailedCreateFuseMountPoint);
        };

        class MountFilesystemManagerException : public SystemException {
            public:
                MountFilesystemManagerException() {}
                MountFilesystemManagerException(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : SystemException(fonction, filename, line), error_number(error_number), mountpoint(mountpoint) {}
            protected:
                qint32 error_number;
                QSharedPointer<QFileInfo> mountpoint;

                DEF_EXCEPTION_WHAT(MountFilesystemManagerException, CoreException, "Error while mounting/unmounting filesystem in " + mountpoint->canonicalFilePath() + "("+ QString(strerror(error_number)) + ")")
            DEC_SERIALIZABLE(MountFilesystemManagerException);
        };

		#define FailMountFilesystemException(error_number, mountpoint, devicePath) FailMountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint, devicePath);
        class FailMountFilesystem : public MountFilesystemManagerException {
            public:
                FailMountFilesystem() {}
                FailMountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint, QSharedPointer<QFileInfo> devicePath) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint), devicePath(devicePath) {}
            protected:
                QSharedPointer<QFileInfo> devicePath;

                DEF_EXCEPTION_WHAT(FailMountFilesystem, MountFilesystemManagerException, "Unable to mount " + devicePath->canonicalFilePath() + " in " + mountpoint->canonicalFilePath() + "("+ QString(strerror(error_number)) + ")")
            DEC_SERIALIZABLE(FailMountFilesystem);
        };

        #define FailUnmountFilesystemException(error_number, mountpoint) FailUnmountFilesystem(__PRETTY_FUNCTION__, __FILE__, __LINE__, error_number, mountpoint);
        class FailUnmountFilesystem : public MountFilesystemManagerException {
            public:
                FailUnmountFilesystem() {}
                FailUnmountFilesystem(QString fonction, QString filename, quint32 line, quint32 error_number, QSharedPointer<QFileInfo> mountpoint) : MountFilesystemManagerException(fonction, filename, line, error_number, mountpoint) {}
            protected:
                DEF_EXCEPTION_WHAT(FailUnmountFilesystem, MountFilesystemManagerException, "Unable to unmount filesystem in " + mountpoint->canonicalFilePath() + "("+ QString(strerror(error_number)) + ")")
            DEC_SERIALIZABLE(FailUnmountFilesystem);
        };

		#define FailedAttachLoopDeviceException(imageFile) FailedAttachLoopDevice(__PRETTY_FUNCTION__, __FILE__, __LINE__, imageFile);
        class FailedAttachLoopDevice : public SystemException {
            public:
                FailedAttachLoopDevice() {}
                FailedAttachLoopDevice(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> imageFile) : SystemException(fonction, filename, line), imageFile(imageFile) {}
            protected:
                QSharedPointer<QFileInfo> imageFile;

                DEF_EXCEPTION_WHAT(FailedAttachLoopDevice, SystemException, "Unable to create loop device for image file " + imageFile->canonicalFilePath())
            DEC_SERIALIZABLE(FailedAttachLoopDevice);
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


#endif // COREEXCEPTION_H

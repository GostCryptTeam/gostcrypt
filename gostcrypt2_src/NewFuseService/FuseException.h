#ifndef FUSEEXCEPTION_H
#define FUSEEXCEPTION_H
#include "NewCore/CoreException.h"

namespace GostCrypt
{
	namespace NewFuseDriver
	{
		void initFuseException();

		class FuseException : public NewCore::GostCryptException {
			public:
				FuseException() {}
				FuseException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
				DEF_EXCEPTION_WHAT(FuseException, NewCore::GostCryptException, "")

			DEC_SERIALIZABLE(FuseException);
		};

		#define FuseTimeoutException(volumePath) FuseTimeout(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		class FuseTimeout : public FuseException {
			public:
                FuseTimeout() {}
                FuseTimeout(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> volumePath) : FuseException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(FuseTimeout, FuseException, "Fuseservice timed out while trying to mount "+ volumePath->absoluteFilePath() + "\n")
            protected:
				QSharedPointer<QFileInfo> volumePath;
			DEC_SERIALIZABLE(FuseTimeout);
		};

		#define FuseControlFileAccessFailedException(fuseMountPoint) FuseControlFileAccessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__, fuseMountPoint);
		class FuseControlFileAccessFailed : public FuseException {
			public:
                FuseControlFileAccessFailed() {}
                FuseControlFileAccessFailed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> fuseMountPoint) : FuseException(fonction, filename, line), fuseMountPoint(fuseMountPoint) {}
                DEF_EXCEPTION_WHAT(FuseControlFileAccessFailed, FuseException, "Fail to open control file to write in : " + fuseMountPoint->absoluteFilePath() + "\n")
            protected:
				QSharedPointer<QFileInfo> fuseMountPoint;
			DEC_SERIALIZABLE(FuseControlFileAccessFailed);
		};

		#define FuseForkFailedException() FuseForkFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		class FuseForkFailed : public FuseException {
			public:
                FuseForkFailed() {}
                FuseForkFailed(QString fonction, QString filename, quint32 line) : FuseException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(FuseForkFailed, FuseException, "The FuseService was not able to fork successfully\n")
			DEC_SERIALIZABLE(FuseForkFailed);
		};

		#define VolumeNotOpenedYetException() VolumeNotOpenedYet(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		class VolumeNotOpenedYet : public FuseException {
			public:
                VolumeNotOpenedYet() {}
                VolumeNotOpenedYet(QString fonction, QString filename, quint32 line) : FuseException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(VolumeNotOpenedYet, FuseException, "Information about the volume were requested before the volume had been opened\n")
			DEC_SERIALIZABLE(VolumeNotOpenedYet);
		};
	}
}

SERIALIZABLE(GostCrypt::NewFuseDriver::FuseException)
SERIALIZABLE(GostCrypt::NewFuseDriver::FuseTimeout)
SERIALIZABLE(GostCrypt::NewFuseDriver::FuseControlFileAccessFailed)
SERIALIZABLE(GostCrypt::NewFuseDriver::FuseForkFailed)
SERIALIZABLE(GostCrypt::NewFuseDriver::VolumeNotOpenedYet)


#endif // FUSEEXCEPTION_H

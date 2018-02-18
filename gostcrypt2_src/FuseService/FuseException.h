#ifndef FUSEEXCEPTION_H
#define FUSEEXCEPTION_H
#include "Core/GostCryptException.h"

namespace GostCrypt
{
	namespace FuseDriver
	{
		/**
		 * @brief
		 *
		 */
		void initFuseException();

		/**
		 * @brief Initialize FuseExceptions to make them serializable, and thus be able to transmit them between processes
		 *
		 */
        class FuseException : public GostCryptException {
			public:
				FuseException() {}
				/**
				 * @brief Base class for all exception concerning Fuse module
				 *
				 * @param fonction Name of the function where the exception was thrown
				 * @param filename Path of the file where the exception was thrown
				 * @param line Line of the file where the exception was thrown
				 */
				FuseException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(FuseException, GostCryptException, "")

			DEC_SERIALIZABLE(FuseException);
		};

		#define FuseTimeoutException(volumePath) FuseTimeout(__PRETTY_FUNCTION__, __FILE__, __LINE__, volumePath);
		/**
		 * @brief Exception thrown when the Fuse mount is still not ready 10 seconds ater the fuse_main function was called
		 *
		 */
		class FuseTimeout : public FuseException {
			public:
                /**
                 * @brief Default constructor used when recreating the exception while deserializing
                 *
                 */
                FuseTimeout() {}
                /**
                 * @brief Conctructor
                 *
				 * @param fonction Name of the function where the exception was thrown
				 * @param filename Path of the file where the exception was thrown
				 * @param line Line of the file where the exception was thrown
                 * @param volumePath path of the volume Fuse was trying to mount
                 */
                FuseTimeout(QString fonction, QString filename, quint32 line, QFileInfo volumePath) : FuseException(fonction, filename, line), volumePath(volumePath) {}
                DEF_EXCEPTION_WHAT(FuseTimeout, FuseException, "Fuseservice timed out while trying to mount "+ volumePath.absoluteFilePath() + "\n")
            protected:
                                QFileInfo volumePath; /**< path of the volume Fuse was trying to mount */
			DEC_SERIALIZABLE(FuseTimeout);
		};

		#define FuseControlFileAccessFailedException(fuseMountPoint) FuseControlFileAccessFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__, fuseMountPoint);
		/**
		 * @brief Exception thrown when the Fuse module fail to access the control file within the fuse mount to write addationnal information on it
		 *
		 */
		class FuseControlFileAccessFailed : public FuseException {
			public:
                /**
                 * @brief Default constructor used when recreating the exception while deserializing                 *
                 */
                FuseControlFileAccessFailed() {}
                /**
                 * @brief Conctructor
                 *
				 * @param fonction Name of the function where the exception was thrown
				 * @param filename Path of the file where the exception was thrown
				 * @param line Line of the file where the exception was thrown
                 * @param fuseMountPoint
                 */
                FuseControlFileAccessFailed(QString fonction, QString filename, quint32 line, QSharedPointer<QFileInfo> fuseMountPoint) : FuseException(fonction, filename, line), fuseMountPoint(fuseMountPoint) {}
                DEF_EXCEPTION_WHAT(FuseControlFileAccessFailed, FuseException, "Fail to open control file to write in : " + fuseMountPoint->absoluteFilePath() + "\n")
            protected:
				QSharedPointer<QFileInfo> fuseMountPoint; /**< TODO: describe */
			DEC_SERIALIZABLE(FuseControlFileAccessFailed);
		};

		#define FuseForkFailedException() FuseForkFailed(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		/**
		 * @brief Exception thrown when the fuse module fail to fork in order to launch the fuse library process
		 *
		 */
		class FuseForkFailed : public FuseException {
			public:
                /**
                 * @brief Default constructor used when recreating the exception while deserializing                 *
                 */
                FuseForkFailed() {}
                /**
                 * @brief Conctructor
                 *
				 * @param fonction Name of the function where the exception was thrown
				 * @param filename Path of the file where the exception was thrown
				 * @param line Line of the file where the exception was thrown
                 */
                FuseForkFailed(QString fonction, QString filename, quint32 line) : FuseException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(FuseForkFailed, FuseException, "The FuseService was not able to fork successfully\n")
			DEC_SERIALIZABLE(FuseForkFailed);
		};

		#define VolumeNotOpenedYetException() VolumeNotOpenedYet(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		/**
		 * @brief Exception thrown when the fuse module try to access a volume file before the volume had been opened
		 *
		 */
		class VolumeNotOpenedYet : public FuseException {
			public:
                /**
                 * @brief Default constructor used when recreating the exception while deserializing                 *
                 */
                VolumeNotOpenedYet() {}
                /**
                 * @brief Conctructor
                 *
				 * @param fonction Name of the function where the exception was thrown
				 * @param filename Path of the file where the exception was thrown
				 * @param line Line of the file where the exception was thrown
                 */
                VolumeNotOpenedYet(QString fonction, QString filename, quint32 line) : FuseException(fonction, filename, line) {}
                DEF_EXCEPTION_WHAT(VolumeNotOpenedYet, FuseException, "Information about the volume were requested before the volume had been opened\n")
			DEC_SERIALIZABLE(VolumeNotOpenedYet);
		};
        #define FailedCreateFuseMountPointException(mountpoint) GostCrypt::FuseDriver::FailedCreateFuseMountPoint(__PRETTY_FUNCTION__, __FILE__, __LINE__, mountpoint);
        /**
         * @brief
         *
         */
        class FailedCreateFuseMountPoint : public FuseException {
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
                FailedCreateFuseMountPoint(QString fonction, QString filename, quint32 line, QFileInfo mountpoint) : FuseException(fonction, filename, line), mountpoint(mountpoint) {}
                DEF_EXCEPTION_WHAT(FailedCreateFuseMountPoint, FuseException, "Creation of fuse mount point " + mountpoint.absoluteFilePath() + " failed\n")
            protected:
                QFileInfo mountpoint; /**< TODO: describe */
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

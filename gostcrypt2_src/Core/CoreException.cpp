#include "CoreException.h"


namespace GostCrypt {
	namespace Core {
        void initCoreException() {
            INIT_SERIALIZE(CoreException);
            INIT_SERIALIZE(FailedOpenFile);
            INIT_SERIALIZE(DeviceNotMounted);
            INIT_SERIALIZE(MissingParam);
            INIT_SERIALIZE(VolumeAlreadyMounted);
            INIT_SERIALIZE(FailedOpenVolume);
            INIT_SERIALIZE(IncorrectSectorSize);
            INIT_SERIALIZE(MountPointUsed);
            INIT_SERIALIZE(MountFilesystemManagerException);
            INIT_SERIALIZE(FailMountFilesystem);
            INIT_SERIALIZE(FailUnmountFilesystem);
            INIT_SERIALIZE(FailedAttachLoopDevice);
            INIT_SERIALIZE(FailedCreateDirectory);
            INIT_SERIALIZE(FailedDetachLoopDevice);
            INIT_SERIALIZE(VolumeNotMounted);
            INIT_SERIALIZE(ContentSizeInvalid);
            INIT_SERIALIZE(InvalidHeaderOffset);
            INIT_SERIALIZE(FormattingSubException);
            INIT_SERIALIZE(ProcessFailed);
            INIT_SERIALIZE(FilesystemNotSupported);
            INIT_SERIALIZE(AlgorithmNotFound);
            INIT_SERIALIZE(IncorrectSudoPassword);
            INIT_SERIALIZE(WorkerProcessCrashed);
			INIT_SERIALIZE(ExceptionFromVolume);
            INIT_SERIALIZE(FailFindFilesystemType);
            INIT_SERIALIZE(InvalidParam);
            INIT_SERIALIZE(IncorrectVolumePassword);
        }

        DEF_SERIALIZABLE(GostCrypt::Core::CoreException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::CoreException & Valeur) {
            out << static_cast<const GostCryptException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::CoreException & Valeur) {
            in >> static_cast<GostCryptException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::DeviceNotMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::DeviceNotMounted & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.device->canonicalFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::DeviceNotMounted & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.device.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::MissingParam)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::MissingParam & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.param;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::MissingParam & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.param;
            return in;
        }

		DEF_SERIALIZABLE(GostCrypt::Core::InvalidParam)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::InvalidParam & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.param;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::InvalidParam & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.param;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::VolumeAlreadyMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::VolumeAlreadyMounted & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::VolumeAlreadyMounted & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.volumePath.reset(new QFileInfo(path));
            return in;
        }

		DEF_SERIALIZABLE(GostCrypt::Core::IncorrectVolumePassword)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::IncorrectVolumePassword & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::IncorrectVolumePassword & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.volumePath.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::VolumeNotMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::VolumeNotMounted & Valeur) {
          out << static_cast<const CoreException&>(Valeur);
          out << Valeur.volumePath->absoluteFilePath();
          return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::VolumeNotMounted & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.volumePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FailedOpenVolume)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailedOpenVolume & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailedOpenVolume & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.volumePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::IncorrectSectorSize)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::IncorrectSectorSize & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::IncorrectSectorSize & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::MountPointUsed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::MountPointUsed & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::MountPointUsed & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.mountpoint.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::MountFilesystemManagerException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::MountFilesystemManagerException & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            out << Valeur.error_number;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::MountFilesystemManagerException & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          in >> Valeur.error_number;
          Valeur.mountpoint.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FailMountFilesystem)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailMountFilesystem & Valeur) {
            out << static_cast<const MountFilesystemManagerException&>(Valeur);
            out << Valeur.devicePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailMountFilesystem & Valeur) {
          QString path;
          in >> static_cast<MountFilesystemManagerException&>(Valeur);
          in >> path;
          Valeur.devicePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::ExceptionFromVolume)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::ExceptionFromVolume & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.message;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::ExceptionFromVolume & Valeur) {
			QString path;
			in >> static_cast<CoreException&>(Valeur);
			in >> Valeur.message;
			return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FailUnmountFilesystem)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailUnmountFilesystem & Valeur) {
            out << static_cast<const MountFilesystemManagerException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailUnmountFilesystem & Valeur) {
          QString path;
          in >> static_cast<MountFilesystemManagerException&>(Valeur);
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FailedAttachLoopDevice)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailedAttachLoopDevice & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.imageFile->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailedAttachLoopDevice & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.imageFile.reset(new QFileInfo(path));
          return in;
        }
        DEF_SERIALIZABLE(GostCrypt::Core::FailedDetachLoopDevice)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailedDetachLoopDevice & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.loopDevice->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailedDetachLoopDevice & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.loopDevice.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::ContentSizeInvalid)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::ContentSizeInvalid & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.size;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::ContentSizeInvalid & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.size;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::InvalidHeaderOffset)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::InvalidHeaderOffset & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.headeroffset;
            out << Valeur.headersize;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::InvalidHeaderOffset & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.headeroffset;
            in >> Valeur.headersize;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FormattingSubException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FormattingSubException & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.subExceptionString;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FormattingSubException & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.subExceptionString;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::ProcessFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::ProcessFailed & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::ProcessFailed & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::FilesystemNotSupported)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FilesystemNotSupported & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.filesystem;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FilesystemNotSupported & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.filesystem;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::AlgorithmNotFound)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::AlgorithmNotFound & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.algorithm;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::AlgorithmNotFound & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.algorithm;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::IncorrectSudoPassword)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::IncorrectSudoPassword & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::IncorrectSudoPassword & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::Core::WorkerProcessCrashed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::WorkerProcessCrashed & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::WorkerProcessCrashed & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

		DEF_SERIALIZABLE(GostCrypt::Core::FailFindFilesystemType)
        QDataStream & operator << (QDataStream & out, const GostCrypt::Core::FailFindFilesystemType & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.devicePath;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::Core::FailFindFilesystemType & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.devicePath;
            return in;
        }

      DEF_SERIALIZABLE(GostCrypt::Core::RandomNumberGeneratorNotRunning)
      QDataStream & operator << (QDataStream & out, const GostCrypt::Core::RandomNumberGeneratorNotRunning & Valeur) {
          out << static_cast<const CoreException&>(Valeur);
          return out;
      }
      QDataStream & operator >> (QDataStream & in, GostCrypt::Core::RandomNumberGeneratorNotRunning & Valeur) {
          in >> static_cast<CoreException&>(Valeur);
          return in;
      }
    }
}

#include "CoreException.h"


namespace GostCrypt {
	namespace NewCore {
        bool initCoreException() {
            INIT_SERIALIZE(CoreException);
            INIT_SERIALIZE(SystemException);
            INIT_SERIALIZE(FailedOpenFile);
            INIT_SERIALIZE(MissingParam);
            INIT_SERIALIZE(VolumeAlreadyMounted);
            INIT_SERIALIZE(FailedOpenVolume);
            INIT_SERIALIZE(IncorrectSectorSize);
            INIT_SERIALIZE(MountPointUsed);
            INIT_SERIALIZE(FailedCreateFuseMountPoint);
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
        }


        DEF_SERIALIZABLE(GostCrypt::NewCore::CoreException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::CoreException & Valeur) {
            out << Valeur.filename << Valeur.line;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CoreException & Valeur) {
            in >> Valeur.filename << Valeur.line;
            return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::SystemException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::SystemException & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::SystemException & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedOpenFile)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedOpenFile & Valeur) {
            out << static_cast<const SystemException&>(Valeur);
            out << Valeur.file.canonicalPath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedOpenFile & Valeur) {
            QString path;
            in >> static_cast<SystemException&>(Valeur);
            in >> path;
            Valeur.file.setFile(path);
            return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::DeviceNotMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::DeviceNotMounted & Valeur) {
            out << static_cast<const SystemException&>(Valeur);
            out << Valeur.device->canonicalFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::DeviceNotMounted & Valeur) {
            QString path;
            in >> static_cast<SystemException&>(Valeur);
            in >> path;
            Valeur.device.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::MissingParam)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::MissingParam & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.param;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::MissingParam & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.param;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::VolumeAlreadyMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::VolumeAlreadyMounted & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::VolumeAlreadyMounted & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.volumePath.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::VolumeNotMounted)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::VolumeNotMounted & Valeur) {
          out << static_cast<const CoreException&>(Valeur);
          out << Valeur.volumePath->absoluteFilePath();
          return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::VolumeNotMounted & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.volumePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedOpenVolume)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedOpenVolume & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedOpenVolume & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.volumePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::IncorrectSectorSize)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::IncorrectSectorSize & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::IncorrectSectorSize & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::MountPointUsed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::MountPointUsed & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::MountPointUsed & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.mountpoint.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedCreateFuseMountPoint)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedCreateFuseMountPoint & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedCreateFuseMountPoint & Valeur) {
          QString path;
          in >> static_cast<CoreException&>(Valeur);
          in >> path;
          Valeur.mountpoint.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::MountFilesystemManagerException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::MountFilesystemManagerException & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            out << Valeur.error_number;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::MountFilesystemManagerException & Valeur) {
          QString path;
          in >> static_cast<SystemException&>(Valeur);
          in >> path;
          in >> Valeur.error_number;
          Valeur.mountpoint.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailMountFilesystem)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailMountFilesystem & Valeur) {
            out << static_cast<const MountFilesystemManagerException&>(Valeur);
            out << Valeur.devicePath->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailMountFilesystem & Valeur) {
          QString path;
          in >> static_cast<MountFilesystemManagerException&>(Valeur);
          in >> path;
          Valeur.devicePath.reset(new QFileInfo(path));
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailUnmountFilesystem)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailUnmountFilesystem & Valeur) {
            out << static_cast<const MountFilesystemManagerException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailUnmountFilesystem & Valeur) {
          QString path;
          in >> static_cast<MountFilesystemManagerException&>(Valeur);
          return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedAttachLoopDevice)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedAttachLoopDevice & Valeur) {
            out << static_cast<const SystemException&>(Valeur);
            out << Valeur.imageFile->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedAttachLoopDevice & Valeur) {
          QString path;
          in >> static_cast<SystemException&>(Valeur);
          in >> path;
          Valeur.imageFile.reset(new QFileInfo(path));
          return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedDetachLoopDevice)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedDetachLoopDevice & Valeur) {
            out << static_cast<const SystemException&>(Valeur);
            out << Valeur.loopDevice->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedDetachLoopDevice & Valeur) {
            QString path;
            in >> static_cast<SystemException&>(Valeur);
            in >> path;
            Valeur.loopDevice.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedCreateDirectory)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedCreateDirectory & Valeur) {
            out << static_cast<const SystemException&>(Valeur);
            out << Valeur.dir;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedCreateDirectory & Valeur) {
            in >> static_cast<SystemException&>(Valeur);
            in >> Valeur.dir;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::ContentSizeInvalid)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::ContentSizeInvalid & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.size;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::ContentSizeInvalid & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.size;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::InvalidHeaderOffset)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::InvalidHeaderOffset & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.headeroffset;
            out << Valeur.headersize;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::InvalidHeaderOffset & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.headeroffset;
            in >> Valeur.headersize;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FormattingSubException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FormattingSubException & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.e;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FormattingSubException & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.e;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::ProcessFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::ProcessFailed & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::ProcessFailed & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FilesystemNotSupported)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FilesystemNotSupported & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.filesystem;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FilesystemNotSupported & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.filesystem;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::AlgorithmNotFound)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::AlgorithmNotFound & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.algorithm;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::AlgorithmNotFound & Valeur) {
            in >> static_cast<CoreException&>(Valeur);
            in >> Valeur.algorithm;
            return in;
        }
    }
}

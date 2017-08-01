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
            out << Valeur.device->canonicalPath();
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
            out << Valeur.volumePath->canonicalFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::VolumeAlreadyMounted & Valeur) {
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            in >> path;
            Valeur.volumePath.reset(new QFileInfo(path));
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedOpenVolume)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedOpenVolume & Valeur) {
            out << static_cast<const CoreException&>(Valeur);
            out << Valeur.volumePath->canonicalFilePath();
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
            QString path;
            in >> static_cast<CoreException&>(Valeur);
            return in;
        }
    }
}

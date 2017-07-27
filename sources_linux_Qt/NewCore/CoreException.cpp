#include "CoreException.h"


namespace GostCrypt {
	namespace NewCore {
		bool initCoreException() {
			INIT_SERIALIZE(CoreException);
			INIT_SERIALIZE(SystemException);
			INIT_SERIALIZE(FailedOpenFile);
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
            out << Valeur.device.canonicalPath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::DeviceNotMounted & Valeur) {
            QString path;
            in >> static_cast<SystemException&>(Valeur);
            in >> path;
            Valeur.device.setFile(path);
            return in;
        }
    }
}

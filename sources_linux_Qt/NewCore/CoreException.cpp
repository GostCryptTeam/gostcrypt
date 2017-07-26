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
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::CoreException & Valeur) {
            return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::SystemException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::SystemException & Valeur) {
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::SystemException & Valeur) {
            return in;
        }
        DEF_SERIALIZABLE(GostCrypt::NewCore::FailedOpenFile)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewCore::FailedOpenFile & Valeur) {
            out << Valeur.file.canonicalPath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewCore::FailedOpenFile & Valeur) {
            QString path;
            in >> path;
            Valeur.file = QFileInfo(path);
            return in;
        }

    }
}

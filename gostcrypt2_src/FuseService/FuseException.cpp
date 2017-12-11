#include "FuseException.h"

namespace GostCrypt
{
	namespace FuseDriver
	{
		void initFuseException()
		{
			INIT_SERIALIZE(FuseException);
			INIT_SERIALIZE(FuseTimeout);
			INIT_SERIALIZE(FuseControlFileAccessFailed);
			INIT_SERIALIZE(FuseForkFailed);
			INIT_SERIALIZE(VolumeNotOpenedYet);
            INIT_SERIALIZE(FailedCreateFuseMountPoint);
		}

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::FuseException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::FuseException & Valeur) {
            out << static_cast<const GostCryptException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::FuseException & Valeur) {
            in >> static_cast<GostCryptException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::FuseTimeout)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::FuseTimeout & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            out << Valeur.volumePath;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::FuseTimeout & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            in >> Valeur.volumePath;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::FuseControlFileAccessFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::FuseControlFileAccessFailed & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            out << Valeur.fuseMountPoint;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::FuseControlFileAccessFailed & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            in >> Valeur.fuseMountPoint;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::FuseForkFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::FuseForkFailed & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::FuseForkFailed & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::VolumeNotOpenedYet)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::VolumeNotOpenedYet & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::VolumeNotOpenedYet & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::FuseDriver::FailedCreateFuseMountPoint)
        QDataStream & operator << (QDataStream & out, const GostCrypt::FuseDriver::FailedCreateFuseMountPoint & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            out << Valeur.mountpoint->absoluteFilePath();
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::FuseDriver::FailedCreateFuseMountPoint & Valeur) {
            QString path;
            in >> static_cast<FuseException&>(Valeur);
            in >> path;
            Valeur.mountpoint.reset(new QFileInfo(path));
            return in;
        }
	}
}

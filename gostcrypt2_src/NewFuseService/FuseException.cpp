#include "FuseException.h"

namespace GostCrypt
{
	namespace NewFuseDriver
	{


		void initFuseException()
		{
			INIT_SERIALIZE(FuseException);
			INIT_SERIALIZE(FuseTimeout);
			INIT_SERIALIZE(FuseControlFileAccessFailed);
			INIT_SERIALIZE(FuseForkFailed);
			INIT_SERIALIZE(VolumeNotOpenedYet);
		}

        DEF_SERIALIZABLE(GostCrypt::NewFuseDriver::FuseException)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewFuseDriver::FuseException & Valeur) {
            out << static_cast<const NewCore::GostCryptException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewFuseDriver::FuseException & Valeur) {
            in >> static_cast<NewCore::GostCryptException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewFuseDriver::FuseTimeout)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewFuseDriver::FuseTimeout & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            out << Valeur.volumePath;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewFuseDriver::FuseTimeout & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            in >> Valeur.volumePath;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewFuseDriver::FuseControlFileAccessFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewFuseDriver::FuseControlFileAccessFailed & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            out << Valeur.fuseMountPoint;
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewFuseDriver::FuseControlFileAccessFailed & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            in >> Valeur.fuseMountPoint;
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewFuseDriver::FuseForkFailed)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewFuseDriver::FuseForkFailed & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewFuseDriver::FuseForkFailed & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            return in;
        }

        DEF_SERIALIZABLE(GostCrypt::NewFuseDriver::VolumeNotOpenedYet)
        QDataStream & operator << (QDataStream & out, const GostCrypt::NewFuseDriver::VolumeNotOpenedYet & Valeur) {
            out << static_cast<const FuseException&>(Valeur);
            return out;
        }
        QDataStream & operator >> (QDataStream & in, GostCrypt::NewFuseDriver::VolumeNotOpenedYet & Valeur) {
            in >> static_cast<FuseException&>(Valeur);
            return in;
        }
	}
}

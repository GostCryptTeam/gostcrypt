#include "GostCryptException.h"
#include "Core/CoreException.h"
#include "FuseService/FuseException.h"
#include "Volume/VolumeException.h"

namespace GostCrypt
{
void initGostCryptException()
{
    INIT_SERIALIZE(GostCryptException);
    INIT_SERIALIZE(SystemException);
    INIT_SERIALIZE(FailedCreateDirectory);
    INIT_SERIALIZE(FailedOpenFile);
    INIT_SERIALIZE(FailedReadFile);
    INIT_SERIALIZE(FailedFlushFile);
    INIT_SERIALIZE(FailedWriteFile);
    INIT_SERIALIZE(FailedLseekFile);
    INIT_SERIALIZE(FailedStatFile);
    INIT_SERIALIZE(FailedMemoryAllocation);
    INIT_SERIALIZE(UnknowException);
    INIT_SERIALIZE(ExternalException);
    INIT_SERIALIZE(InvalidParameter);
    Core::initCoreException();
    FuseDriver::initFuseException();
    Volume::initVolumeException();
}

DEF_SERIALIZABLE(GostCrypt::GostCryptException)
QDataStream& operator << (QDataStream& out, const GostCrypt::GostCryptException& Valeur)
{
    out << Valeur.filename;
    out << Valeur.line;
    out << Valeur.fonction;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::GostCryptException& Valeur)
{
    in >> Valeur.filename;
    in >> Valeur.line;
    in >> Valeur.fonction;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::SystemException)
QDataStream& operator << (QDataStream& out, const GostCrypt::SystemException& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::SystemException& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedOpenFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedOpenFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedOpenFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedWriteFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedWriteFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedWriteFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedFlushFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedFlushFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedFlushFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedLseekFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedLseekFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedLseekFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedStatFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedStatFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedStatFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedReadFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedReadFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file.canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedReadFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.setFile(path);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedCreateDirectory)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedCreateDirectory& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.dir;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedCreateDirectory& Valeur)
{
    in >> static_cast<SystemException&>(Valeur);
    in >> Valeur.dir;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedMemoryAllocation)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedMemoryAllocation& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedMemoryAllocation& Valeur)
{
    in >> static_cast<SystemException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::UnknowException)
QDataStream& operator << (QDataStream& out, const GostCrypt::UnknowException& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::UnknowException& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    return in;
}

DEF_SERIALIZABLE(GostCrypt::ExternalException)
QDataStream& operator << (QDataStream& out, const GostCrypt::ExternalException& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    out << Valeur.info;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::ExternalException& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    in >> Valeur.info;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::InvalidParameter)
QDataStream& operator << (QDataStream& out, const GostCrypt::InvalidParameter& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    out << Valeur.param;
    out << Valeur.comment;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::InvalidParameter& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    in >> Valeur.param;
    in >> Valeur.comment;
    return in;
}

}

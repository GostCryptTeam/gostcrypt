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
    INIT_SERIALIZE(FailedWriteFile);
    INIT_SERIALIZE(FailedLseekFile);
    INIT_SERIALIZE(FailedStatFile);
    INIT_SERIALIZE(FailedMemoryAllocation);
    INIT_SERIALIZE(IncorrectParameter);


    //add here all systemException
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
    out << Valeur.file->canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedOpenFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.reset(new QFileInfo(path));
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedWriteFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedWriteFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file->canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedWriteFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.reset(new QFileInfo(path));
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedLseekFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedLseekFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file->canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedLseekFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.reset(new QFileInfo(path));
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedStatFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedStatFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file->canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedStatFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.reset(new QFileInfo(path));
    return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedReadFile)
QDataStream& operator << (QDataStream& out, const GostCrypt::FailedReadFile& Valeur)
{
    out << static_cast<const SystemException&>(Valeur);
    out << Valeur.file->canonicalPath();
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::FailedReadFile& Valeur)
{
    QString path;
    in >> static_cast<SystemException&>(Valeur);
    in >> path;
    Valeur.file.reset(new QFileInfo(path));
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

DEF_SERIALIZABLE(GostCrypt::IncorrectParameter)
QDataStream& operator << (QDataStream& out, const GostCrypt::IncorrectParameter& Valeur)
{
    out << static_cast<const GostCryptException&>(Valeur);
    out << Valeur.comment;
    return out;
}
QDataStream& operator >> (QDataStream& in, GostCrypt::IncorrectParameter& Valeur)
{
    in >> static_cast<GostCryptException&>(Valeur);
    in >> Valeur.comment;
    return in;
}

}

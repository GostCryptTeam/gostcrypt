#include "GostCryptException.h"
#include "Core/CoreException.h"
#include "FuseService/FuseException.h"

namespace GostCrypt {
void initGostCryptException()
{
    INIT_SERIALIZE(GostCryptException);
    INIT_SERIALIZE(SystemException);
    //add here all systemException
    Core::initCoreException();
    FuseDriver::initFuseException();
}

DEF_SERIALIZABLE(GostCrypt::GostCryptException)
QDataStream & operator << (QDataStream & out, const GostCrypt::GostCryptException & Valeur) {
    out << Valeur.filename;
    out << Valeur.line;
    out << Valeur.fonction;
    return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::GostCryptException & Valeur) {
    in >> Valeur.filename;
    in >> Valeur.line;
    in >> Valeur.fonction;
    return in;
}

DEF_SERIALIZABLE(GostCrypt::SystemException)
QDataStream & operator << (QDataStream & out, const GostCrypt::SystemException & Valeur) {
  out << static_cast<const GostCryptException&>(Valeur);
  return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::SystemException & Valeur) {
  in >> static_cast<GostCryptException&>(Valeur);
                          return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedOpenFile)
QDataStream & operator << (QDataStream & out, const GostCrypt::FailedOpenFile & Valeur) {
  out << static_cast<const SystemException&>(Valeur);
  out << Valeur.file.canonicalPath();
  return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::FailedOpenFile & Valeur) {
  QString path;
  in >> static_cast<SystemException&>(Valeur);
  in >> path;
  Valeur.file.setFile(path);
  return in;
}

DEF_SERIALIZABLE(GostCrypt::FailedCreateDirectory)
QDataStream & operator << (QDataStream & out, const GostCrypt::FailedCreateDirectory & Valeur) {
  out << static_cast<const SystemException&>(Valeur);
  out << Valeur.dir;
  return out;
}
QDataStream & operator >> (QDataStream & in, GostCrypt::FailedCreateDirectory & Valeur) {
  in >> static_cast<SystemException&>(Valeur);
  in >> Valeur.dir;
  return in;
}

}

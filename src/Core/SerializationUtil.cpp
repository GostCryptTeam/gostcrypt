#include "SerializationUtil.h"

QDataStream& operator<< (QDataStream& out, const QFileInfo& Valeur)
{
    if(Valeur.filePath().isEmpty())
        out << QString();
    else
        out << Valeur.absoluteFilePath();

    return out;
}

QDataStream& operator>> (QDataStream& in, QFileInfo& Valeur)
{
    QString path;
    in >> path;
    Valeur = QFileInfo(path);
    return in;
}

DEF_SERIALIZABLE(QByteArray)

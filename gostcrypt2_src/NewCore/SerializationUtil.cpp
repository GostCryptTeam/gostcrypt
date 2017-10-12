#include "SerializationUtil.h"

QDataStream & operator<< (QDataStream & out, const QFileInfo & Valeur){
    out << Valeur.absoluteFilePath();
    return out;
}

QDataStream & operator>> (QDataStream & in, QFileInfo & Valeur){
    QString path;
    in >> path;
    Valeur = QFileInfo(path);
    return in;
}

DEF_SERIALIZABLE(QList<QSharedPointer<QFileInfo>>)
DEF_SERIALIZABLE(QFileInfo)
DEF_SERIALIZABLE(QByteArray)

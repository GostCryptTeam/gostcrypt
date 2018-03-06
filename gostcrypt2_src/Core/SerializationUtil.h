#ifndef SERIALIZEUTIL_H
#define SERIALIZEUTIL_H

#include <QDataStream>
#include <QSharedPointer>
#include <QFileInfo>

#define SERIALIZABLE(ClassName) \
    Q_DECLARE_METATYPE(ClassName) \
    Q_DECLARE_METATYPE(QSharedPointer<ClassName>)

#define DEC_SERIALIZABLE(ClassName) \
friend QDataStream & operator<< (QDataStream & out, const ClassName & Valeur); \
friend QDataStream & operator>> (QDataStream & in, ClassName & Valeur); \
friend QDataStream & operator>> (QDataStream & in, QSharedPointer<ClassName> & Valeur); \
friend QDataStream & operator<< (QDataStream & out, const QSharedPointer<ClassName> & Valeur)

#define DEF_SERIALIZABLE(ClassName) \
QDataStream & operator<< (QDataStream & out, const QSharedPointer<ClassName> & Valeur) { \
    if(!Valeur) { \
        out << false; \
    } else { \
        out << true; \
        out << *Valeur; \
    } \
    return out; \
} \
QDataStream & operator>> (QDataStream & in, QSharedPointer<ClassName> & Valeur) { \
    bool isPresent; \
    in >> isPresent; \
    if(isPresent) { \
        Valeur.reset(new ClassName); \
        in >> *Valeur; \
    } \
    return in; \
}

#define INIT_SERIALIZE(ClassName) \
    qRegisterMetaTypeStreamOperators<ClassName>(#ClassName); \
    qRegisterMetaTypeStreamOperators<QSharedPointer<ClassName>>("QSharedPointer<"#ClassName">"); \
    qMetaTypeId<ClassName>(); \
    qMetaTypeId<QSharedPointer<ClassName>>()


/**
 * @brief Serializing operator for the QFileInfo object used for inter-process communication
 *
 * @param out QDataStream in which the object is serialized
 * @param Valeur Object to serialize
 * @return out
 */
QDataStream& operator<< (QDataStream& out, const QFileInfo& Valeur);
/**
 * @brief Deserializing operator for the QFileInfo object used for inter-process communication
 *
 * @param in QDataStream from which the object is serialized
 * @param Valeur Object to deserialized
 * @return in
 */
QDataStream& operator>> (QDataStream& in, QFileInfo& Valeur);
/**
 * @brief Serializing operator for the QSharedPointer<QByteArray> object used for inter-process communication
 *
 * @param out QDataStream in which the object is serialized
 * @param Valeur Object to serialize
 * @return out
 */
QDataStream& operator<< (QDataStream& out, const QSharedPointer<QByteArray>& Valeur);
/**
 * @brief Deserializing operator for the QSharedPointer<QByteArray> object used for inter-process communication
 *
 * @param in QDataStream from which the object is serialized
 * @param Valeur Object to deserialized
 * @return in
 */
QDataStream& operator>> (QDataStream& in, QSharedPointer<QByteArray>& Valeur);
Q_DECLARE_METATYPE(QSharedPointer<QByteArray>)


#endif // SERIALIZEUTIL_H

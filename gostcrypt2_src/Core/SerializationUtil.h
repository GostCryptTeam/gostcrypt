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


QDataStream & operator<< (QDataStream & out, const QFileInfo & Valeur);
QDataStream & operator>> (QDataStream & in, QFileInfo & Valeur);
QDataStream & operator>> (QDataStream & in, QSharedPointer<QFileInfo> & Valeur);
QDataStream & operator<< (QDataStream & out, const QSharedPointer<QFileInfo> & Valeur);
QDataStream & operator>> (QDataStream & in, QSharedPointer<QByteArray> & Valeur);
QDataStream & operator<< (QDataStream & out, const QSharedPointer<QByteArray> & Valeur);
QDataStream & operator>> (QDataStream & in, QSharedPointer <QList<QSharedPointer<QFileInfo>>> & Valeur);
QDataStream & operator<< (QDataStream & out, const QSharedPointer <QList<QSharedPointer<QFileInfo>>> & Valeur);
Q_DECLARE_METATYPE(QSharedPointer<QFileInfo>)
Q_DECLARE_METATYPE(QSharedPointer<QByteArray>)
Q_DECLARE_METATYPE(QSharedPointer <QList<QSharedPointer<QFileInfo>>>)


#endif // SERIALIZEUTIL_H

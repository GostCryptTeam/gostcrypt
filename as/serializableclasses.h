#ifndef SERIALIZABLECLASSES_H
#define SERIALIZABLECLASSES_H

#include <QDataStream>
#include <QDebug>

#define DEF_SERIALIZABLE(ClassName) \
	QDataStream & operator >> (QDataStream & in, QSharedPointer<ClassName> & Valeur) { \
		Valeur.reset(new ClassName); \
		in >> *Valeur; \
		return in; \
	} \
	QDataStream & operator << (QDataStream & out, const QSharedPointer<ClassName> & Valeur) { \
		out << *Valeur; \
		return out; \
	}

#define DEC_SERIALIZABLE(ClassName) \
	QDataStream & operator << (QDataStream & out, const ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, ClassName & Valeur); \
	QDataStream & operator << (QDataStream & out, const QSharedPointer<ClassName> & Valeur); \
	QDataStream & operator >> (QDataStream & in, QSharedPointer<ClassName> & Valeur); \
	Q_DECLARE_METATYPE(ClassName) \
	Q_DECLARE_METATYPE(QSharedPointer<ClassName>)

#define INIT_SERIALIZABLE(ClassName) \
    qRegisterMetaTypeStreamOperators<ClassName>(#ClassName); \
    qRegisterMetaTypeStreamOperators<QSharedPointer<ClassName>>("QSharedPointer<"#ClassName">"); \
	qMetaTypeId<ClassName>(); \
	qMetaTypeId<QSharedPointer<ClassName>>()

struct BaseRequest {};
struct MaxRequest : BaseRequest {
	qint32 a;
	qint32 b;
	void print() {
		qDebug() << QString() + "max(" + QString::number(a) + "," + QString::number(b) + ")";

	}
};
struct MinRequest : BaseRequest {
	qint32 a;
	qint32 b;
	void print() {
		qDebug() << QString() + "min(" + QString::number(a) + "," + QString::number(b) + ")";
	}
};
struct ExitRequest : BaseRequest {};
struct BaseResponse {};
struct MaxResponse : BaseResponse {
	qint32 res;
	void print() {
		qDebug() << QString() + "max = " + QString::number(res);
	}
};
struct MinResponse : BaseResponse {
	qint32 res;
	void print() {
		qDebug() << QString() + "min = " + QString::number(res);
	}
};

DEC_SERIALIZABLE(BaseRequest)
DEC_SERIALIZABLE(MaxRequest)
DEC_SERIALIZABLE(MinRequest)
DEC_SERIALIZABLE(ExitRequest)
DEC_SERIALIZABLE(BaseResponse)
DEC_SERIALIZABLE(MaxResponse)
DEC_SERIALIZABLE(MinResponse)

void initSerializables();

#endif // SERIALIZABLECLASSES_H

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

#define INIT_SERIALIZE(ClassName) \
    qRegisterMetaTypeStreamOperators<ClassName>(#ClassName); \
    qRegisterMetaTypeStreamOperators<QSharedPointer<ClassName>>("QSharedPointer<"#ClassName">"); \
	qMetaTypeId<ClassName>(); \
	qMetaTypeId<QSharedPointer<ClassName>>()

struct BaseClass {
	qint32 a;
	void print() {
		qDebug() << a;
	}
};

struct ChildCLass1 : BaseClass {
	qint32 b;
	void print() {
		BaseClass::print();
		qDebug() << b;
	}
};

struct ChildCLass2 : BaseClass {
	qreal c;
	void print() {
		BaseClass::print();
		qDebug() << c;
	}
};

DEC_SERIALIZABLE(BaseClass)
DEC_SERIALIZABLE(ChildCLass1)
DEC_SERIALIZABLE(ChildCLass2)

#endif // SERIALIZABLECLASSES_H

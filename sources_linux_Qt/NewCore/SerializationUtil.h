#ifndef SERIALIZEUTIL_H
#define SERIALIZEUTIL_H

#define SERIALIZABLE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
	QDataStream & operator << (QDataStream & out, const ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, QSharedPointer<ClassName> & Valeur) { \
		Valeur.reset(new ClassName); \
		in >> *Valeur; \
		return in; \
	} \
	QDataStream & operator << (QDataStream & out, const QSharedPointer<ClassName> & Valeur) { \
		out << *Valeur; \
		return out; \
	} \
	Q_DECLARE_METATYPE(QSharedPointer<ClassName>)

#define INIT_SERIALIZE(ClassName) \
	qRegisterMetaTypeStreamOperators<ClassName>("ClassName"); \
	qRegisterMetaTypeStreamOperators<QSharedPointer<ClassName>>("QSharedPointer<ClassName>"); \
	qMetaTypeId<ClassName>(); \
	qMetaTypeId<QSharedPointer<ClassName>>()

#endif // SERIALIZEUTIL_H

#ifndef SERIALIZEUTIL_H
#define SERIALIZEUTIL_H

#define SERIALIZABLE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
	QDataStream & operator << (QDataStream & out, const ClassName & Valeur); \
	QDataStream & operator >> (QDataStream & in, ClassName & Valeur)

#define INIT_SERIALIZE(ClassName) \
	qRegisterMetaTypeStreamOperators<ClassName>("ClassName"); \
	qMetaTypeId<ClassName>()

#endif // SERIALIZEUTIL_H

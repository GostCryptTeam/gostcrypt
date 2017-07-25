#ifndef SERIALIZEUTIL_H
#define SERIALIZEUTIL_H

#define SERIALIZABLE(ClassName) \
	Q_DECLARE_METATYPE(ClassName) \
    QDataStream & operator<< (QDataStream & out, const ClassName & Valeur); \
    QDataStream & operator>> (QDataStream & in, ClassName & Valeur); \
    QDataStream & operator>> (QDataStream & in, QSharedPointer<ClassName> & Valeur); \
    QDataStream & operator<< (QDataStream & out, const QSharedPointer<ClassName> & Valeur); \
	Q_DECLARE_METATYPE(QSharedPointer<ClassName>)


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
	qRegisterMetaTypeStreamOperators<ClassName>("ClassName"); \
	qRegisterMetaTypeStreamOperators<QSharedPointer<ClassName>>("QSharedPointer<ClassName>"); \
	qMetaTypeId<ClassName>(); \
	qMetaTypeId<QSharedPointer<ClassName>>()

#endif // SERIALIZEUTIL_H

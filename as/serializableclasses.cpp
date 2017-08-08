#include "serializableclasses.h"

DEF_SERIALIZABLE(BaseRequest)
DEF_SERIALIZABLE(MaxRequest)
DEF_SERIALIZABLE(MinRequest)
DEF_SERIALIZABLE(ExitRequest)
DEF_SERIALIZABLE(BaseResponse)
DEF_SERIALIZABLE(MaxResponse)
DEF_SERIALIZABLE(MinResponse)

QDataStream & operator << (QDataStream & out, const BaseRequest & Valeur) {
	(void)Valeur;
	return out;
}
QDataStream & operator >> (QDataStream & in, BaseRequest & Valeur) {
	(void)Valeur;
	return in;
}

QDataStream & operator << (QDataStream & out, const MaxRequest & Valeur) {
	out << static_cast<const BaseRequest&>(Valeur);
	out << Valeur.a;
	out << Valeur.b;
	return out;
}
QDataStream & operator >> (QDataStream & in, MaxRequest & Valeur) {
	in >> static_cast<BaseRequest&>(Valeur);
	in >> Valeur.a;
	in >> Valeur.b;
	return in;
}

QDataStream & operator << (QDataStream & out, const MinRequest & Valeur) {
	out << static_cast<const BaseRequest&>(Valeur);
	out << Valeur.a;
	out << Valeur.b;
	return out;
}
QDataStream & operator >> (QDataStream & in, MinRequest & Valeur) {
	in >> static_cast<BaseRequest&>(Valeur);
	in >> Valeur.a;
	in >> Valeur.b;
	return in;
}

QDataStream & operator << (QDataStream & out, const BaseResponse & Valeur) {
	(void)Valeur;
	return out;
}
QDataStream & operator >> (QDataStream & in, BaseResponse & Valeur) {
	(void)Valeur;
	return in;
}

QDataStream & operator << (QDataStream & out, const MaxResponse & Valeur) {
	out << static_cast<const BaseResponse&>(Valeur);
	out << Valeur.res;
	return out;
}
QDataStream & operator >> (QDataStream & in, MaxResponse & Valeur) {
	in >> static_cast<BaseResponse&>(Valeur);
	in >> Valeur.res;
	return in;
}

QDataStream & operator << (QDataStream & out, const MinResponse & Valeur) {
	out << static_cast<const BaseResponse&>(Valeur);
	out << Valeur.res;
	return out;
}
QDataStream & operator >> (QDataStream & in, MinResponse & Valeur) {
	in >> static_cast<BaseResponse&>(Valeur);
	in >> Valeur.res;
	return in;
}

QDataStream & operator << (QDataStream & out, const ExitRequest & Valeur) {
	out << static_cast<const BaseRequest&>(Valeur);
	return out;
}
QDataStream & operator >> (QDataStream & in, ExitRequest & Valeur) {
	in >> static_cast<BaseRequest&>(Valeur);
	return in;
}

void initSerializables()
{
	INIT_SERIALIZABLE(BaseRequest);
	INIT_SERIALIZABLE(MaxRequest);
	INIT_SERIALIZABLE(MinRequest);
	INIT_SERIALIZABLE(ExitRequest);
	INIT_SERIALIZABLE(BaseResponse);
	INIT_SERIALIZABLE(MaxResponse);
	INIT_SERIALIZABLE(MinResponse);
}

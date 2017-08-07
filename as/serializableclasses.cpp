#include "serializableclasses.h"

DEF_SERIALIZABLE(BaseClass)
DEF_SERIALIZABLE(ChildCLass1)
DEF_SERIALIZABLE(ChildCLass2)

QDataStream & operator << (QDataStream & out, const BaseClass & Valeur) {
	out << Valeur.a;
	return out;
}
QDataStream & operator >> (QDataStream & in, BaseClass & Valeur) {
	in >> Valeur.a;
	return in;
}

QDataStream & operator << (QDataStream & out, const ChildCLass1 & Valeur) {
	out << static_cast<const BaseClass&>(Valeur);
	out << Valeur.b;
	return out;
}
QDataStream & operator >> (QDataStream & in, ChildCLass1 & Valeur) {
	in >> static_cast<BaseClass&>(Valeur);
	in >> Valeur.b;
	return in;
}

QDataStream & operator << (QDataStream & out, const ChildCLass2 & Valeur) {
	out << static_cast<const BaseClass&>(Valeur);
	out << Valeur.c;
	return out;
}
QDataStream & operator >> (QDataStream & in, ChildCLass2 & Valeur) {
	in >> static_cast<BaseClass&>(Valeur);
	in >> Valeur.c;
	return in;
}

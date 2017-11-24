/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Serializable.h"
#include "SerializerFactory.h"

namespace GostCrypt
{
	string Serializable::DeserializeHeader (QSharedPointer <Stream> stream)
	{
		Serializer sr (stream);
		return sr.DeserializeString ("SerializableName");
	}

	Serializable *Serializable::DeserializeNew (QSharedPointer <Stream> stream)
	{
		string name = Serializable::DeserializeHeader (stream);
		Serializable *serializable = SerializerFactory::GetNewSerializable (name);
		serializable->Deserialize (stream);

		return serializable;
	}

	void Serializable::Serialize (QSharedPointer <Stream> stream) const
	{
		Serializer sr (stream);
		Serializable::SerializeHeader (sr, SerializerFactory::GetName (typeid (*this)));
	}

	void Serializable::SerializeHeader (Serializer &serializer, const string &name)
	{
		serializer.Serialize ("SerializableName", name);
	}
}

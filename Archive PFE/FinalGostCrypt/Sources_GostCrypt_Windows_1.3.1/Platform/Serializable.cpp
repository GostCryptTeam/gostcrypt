/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

/**
*
*	\file Serializable.cpp
*	\brief
*	\version
*	\date
*
*/

#include "Serializable.h"
#include "SerializerFactory.h"

namespace GostCrypt
{
/**
 *
 *	\fn string Serializable::DeserializeHeader (shared_ptr <Stream> stream)
 *	\brief
 *	\param[in] shared_ptr <Stream>
 *	\return
 *
 */
	string Serializable::DeserializeHeader (shared_ptr <Stream> stream)
	{
		Serializer sr (stream);
		return sr.DeserializeString ("SerializableName");
	}

/**
 *
 *	\fn Serializable *Serializable::DeserializeNew (shared_ptr <Stream> stream)
 *	\brief
 *	\param[in] shared_ptr <Stream>
 *	\return
 *
 */
	Serializable *Serializable::DeserializeNew (shared_ptr <Stream> stream)
	{
		string name = Serializable::DeserializeHeader (stream);
		Serializable *serializable = SerializerFactory::GetNewSerializable (name);
		serializable->Deserialize (stream);

		return serializable;
	}

	void Serializable::Serialize (shared_ptr <Stream> stream) const
	{
		Serializer sr (stream);
		Serializable::SerializeHeader (sr, SerializerFactory::GetName (typeid (*this)));
	}

/**
 *
 *	\fn void Serializable::SerializeHeader (Serializer &serializer, const string &name)
 *	\brief
 *	\param[in] const string
 *	\param[in] Serializer &serializer
 *	\return
 *
 */
	void Serializable::SerializeHeader (Serializer &serializer, const string &name)
	{
		serializer.Serialize ("SerializableName", name);
	}
}

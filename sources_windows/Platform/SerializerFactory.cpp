/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

/**
*
*	\file SerializerFactory.cpp
*	\brief
*	\version
*	\date
*
*/

#include <stdexcept>
#include "SerializerFactory.h"

namespace GostCrypt
{
/**
 *
 *	\fn void SerializerFactory::Deinitialize ()
 *	\brief
 *	\return
 *
 */
	void SerializerFactory::Deinitialize ()
	{
		if (--UseCount == 0)
		{
			delete NameToTypeMap;
			delete TypeToNameMap;
		}
	}

/**
 *
 *	\fn string SerializerFactory::GetName (const type_info &typeInfo)
 *	\brief
 *	\param[in] const type_info
 *	\return
 *
 */
	string SerializerFactory::GetName (const type_info &typeInfo)
	{
		string typeName = StringConverter::GetTypeName (typeInfo);
		if (TypeToNameMap->find (typeName) == TypeToNameMap->end())
			throw std::runtime_error (SRC_POS);

		return (*TypeToNameMap)[typeName];
	}

/**
 *
 *	\fn Serializable *SerializerFactory::GetNewSerializable (const string &typeName)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	Serializable *SerializerFactory::GetNewSerializable (const string &typeName)
	{
		if (NameToTypeMap->find (typeName) == NameToTypeMap->end())
			throw std::runtime_error (SRC_POS);

		return (*NameToTypeMap)[typeName].GetNewPtr();
	}

/**
 *
 *	\fn void SerializerFactory::Initialize ()
 *	\brief
 *	\return
 *
 */
	void SerializerFactory::Initialize ()
	{
		if (UseCount == 0)
		{
			NameToTypeMap = new map <string, SerializerFactory::MapEntry>;
			TypeToNameMap = new map <string, string>;
		}

		++UseCount;
	}

	map <string, SerializerFactory::MapEntry> *SerializerFactory::NameToTypeMap;
	map <string, string> *SerializerFactory::TypeToNameMap;
	int SerializerFactory::UseCount;
}

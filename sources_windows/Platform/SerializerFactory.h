/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_SerializerFactory
#define GST_HEADER_Platform_SerializerFactory

#include <typeinfo>
#include "PlatformBase.h"
#include "StringConverter.h"

namespace GostCrypt
{
	class Serializable;

	class SerializerFactory
	{
	public:
		~SerializerFactory ();

		static void Deinitialize ();
		static string GetName (const type_info &typeInfo);
		static Serializable *GetNewSerializable (const string &typeName);
		static void Initialize ();

		struct MapEntry
		{
			MapEntry () { }
			MapEntry (const string &typeName, Serializable* (*getNewPtr) ())
				: TypeName (typeName), GetNewPtr (getNewPtr) { }

			MapEntry &operator= (const MapEntry &right)
			{
				TypeName = right.TypeName;
				GetNewPtr = right.GetNewPtr;
				return *this;
			}

			string TypeName;
			Serializable* (*GetNewPtr) ();
		};

		static map <string, MapEntry> *NameToTypeMap;
		static map <string, string> *TypeToNameMap;

	protected:
		SerializerFactory ();

		static int UseCount;
	};

}

#define GST_SERIALIZER_FACTORY_ADD_EXCEPTION_SET(TYPE) \
	struct TYPE##SerializerFactoryInitializer \
	{ \
		TYPE##SerializerFactoryInitializer () \
		{ \
			SerializerFactory::Initialize(); \
			GST_EXCEPTION_SET; \
		} \
		~TYPE##SerializerFactoryInitializer () \
		{ \
			SerializerFactory::Deinitialize(); \
		} \
	}; \
	static TYPE##SerializerFactoryInitializer TYPE##SerializerFactoryInitializer

#define GST_SERIALIZER_FACTORY_ADD_CLASS(TYPE) \
	struct TYPE##SerializerFactoryInitializer \
	{ \
		TYPE##SerializerFactoryInitializer () \
		{ \
			SerializerFactory::Initialize(); \
			GST_SERIALIZER_FACTORY_ADD (TYPE); \
		} \
		~TYPE##SerializerFactoryInitializer () \
		{ \
			SerializerFactory::Deinitialize(); \
		} \
	}; \
	static TYPE##SerializerFactoryInitializer TYPE##SerializerFactoryInitializerInst

#define GST_SERIALIZER_FACTORY_ADD(TYPE) \
	(*SerializerFactory::NameToTypeMap)[#TYPE] = SerializerFactory::MapEntry (StringConverter::GetTypeName (typeid (TYPE)), &TYPE::GetNewSerializable); \
	(*SerializerFactory::TypeToNameMap)[StringConverter::GetTypeName (typeid (TYPE))] = #TYPE


#endif // GST_HEADER_Platform_SerializerFactory

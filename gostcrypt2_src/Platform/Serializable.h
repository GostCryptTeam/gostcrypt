/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_Serializable
#define GST_HEADER_Platform_Serializable

#include <stdexcept>
#include "PlatformBase.h"
#include "ForEach.h"
#include "Serializer.h"
#include "SerializerFactory.h"

namespace GostCrypt
{
	class Serializable
	{
	public:
		virtual ~Serializable () { }

		virtual void Deserialize (QSharedPointer <Stream> stream) = 0;
		static string DeserializeHeader (QSharedPointer <Stream> stream);
		static Serializable *DeserializeNew (QSharedPointer <Stream> stream);
		
		template <class T> 
		static QSharedPointer <T> DeserializeNew (QSharedPointer <Stream> stream)
		{
			QSharedPointer <T> p (dynamic_cast <T *> (DeserializeNew (stream)));
			if (!p)
				throw std::runtime_error (SRC_POS);
			return p;
		}

		template <class T> 
		static void DeserializeList (QSharedPointer <Stream> stream, list < QSharedPointer <T> > &dataList)
		{
			if (DeserializeHeader (stream) != string ("list<") + SerializerFactory::GetName (typeid (T)) + ">")
				throw std::runtime_error (SRC_POS);

			Serializer sr (stream);
			uint64 listSize;
			sr.Deserialize ("ListSize", listSize);

			for (size_t i = 0; i < listSize; i++)
			{
				QSharedPointer <T> p (dynamic_cast <T *> (DeserializeNew (stream)));
				if (!p)
					throw std::runtime_error (SRC_POS);
				dataList.push_back (p);
			}
		}

		virtual void Serialize (QSharedPointer <Stream> stream) const;

		template <class T>
		static void SerializeList (QSharedPointer <Stream> stream, const list < QSharedPointer <T> > &dataList)
		{
			Serializer sr (stream);
			SerializeHeader (sr, string ("list<") + SerializerFactory::GetName (typeid (T)) + ">");

			sr.Serialize ("ListSize", (uint64) dataList.size());
			foreach_ref (const T &item, dataList)
				item.Serialize (stream);
		}

		static void SerializeHeader (Serializer &serializer, const string &name);

	protected:
		Serializable () { }
	};
}

#define GST_SERIALIZABLE(TYPE) \
	static Serializable *GetNewSerializable () { return new TYPE(); } \
	virtual void Deserialize (QSharedPointer <Stream> stream); \
	virtual void Serialize (QSharedPointer <Stream> stream) const

#endif // GST_HEADER_Platform_Serializable

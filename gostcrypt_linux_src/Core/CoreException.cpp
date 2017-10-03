/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "CoreException.h"
#include "Platform/SerializerFactory.h"

namespace GostCrypt
{
	void ElevationFailed::Deserialize (shared_ptr <Stream> stream)
	{
		ExecutedProcessFailed::Deserialize (stream);
	}

	void ElevationFailed::Serialize (shared_ptr <Stream> stream) const
	{
		ExecutedProcessFailed::Serialize (stream);
	}

#define GST_EXCEPTION(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)
#undef GST_EXCEPTION_NODECL
#define GST_EXCEPTION_NODECL(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)

	GST_SERIALIZER_FACTORY_ADD_EXCEPTION_SET (CoreException);
}

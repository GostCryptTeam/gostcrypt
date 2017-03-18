/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_FatFormatter
#define GST_HEADER_Core_FatFormatter

#include "Platform/Platform.h"

namespace GostCrypt
{
	class FatFormatter
	{
	public:
		struct WriteSectorCallback
		{
			virtual ~WriteSectorCallback () { }
			virtual bool operator() (const BufferPtr &sector) = 0;
		};

		static void Format (WriteSectorCallback &writeSector, uint64 deviceSize, uint32 clusterSize, uint32 sectorSize);
	};
}

#endif // GST_HEADER_Core_FatFormatter

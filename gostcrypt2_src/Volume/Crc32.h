/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Crc32
#define GST_HEADER_Encryption_Crc32

#include "Crypto/Crc.h"
#include "Platform/Buffer.h"

namespace GostCrypt
{
namespace Volume {

	class Crc32
	{
	public:
        Crc32 () : CrcValue (0xffffFFFF) { }
        virtual ~Crc32 () { }

		quint32 Get () const { return CrcValue ^ 0xffffFFFF; }

		quint32 Process (quint8 data)
		{
			return CrcValue = crc_32_tab[(quint8) (CrcValue ^ data)] ^ (CrcValue >> 8);
		}

        static quint32 ProcessBuffer (const BufferPtr &buffer)
		{
			return ::GetCrc32 (const_cast<quint8 *> (buffer.Get()), static_cast<int> (buffer.Size()));
		}

	protected:
		quint32 CrcValue;

	private:
		Crc32 (const Crc32 &);
		Crc32 &operator= (const Crc32 &);
	};
}
}

#endif // GST_HEADER_Encryption_Crc32

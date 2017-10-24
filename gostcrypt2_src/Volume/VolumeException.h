/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_VolumeExceptions
#define GST_HEADER_Volume_VolumeExceptions

#include "Platform/Platform.h"

namespace GostCrypt
{
namespace Volume {

	struct VolumeException : public Exception
	{
	protected:
		VolumeException ();
		VolumeException (const string &message);
		VolumeException (const string &message, const wstring &subject);
	};

#define GST_EXCEPTION(NAME) GST_EXCEPTION_DECL(NAME,VolumeException)

#undef GST_EXCEPTION_SET
#define GST_EXCEPTION_SET \
	GST_EXCEPTION (HigherVersionRequired); \
	GST_EXCEPTION (OlderVersionRequired); \
	GST_EXCEPTION (KeyfilePathEmpty); \
	GST_EXCEPTION (NextDeprecated); \
	GST_EXCEPTION (MissingVolumeData); \
	GST_EXCEPTION (MountedVolumeInUse); \
	GST_EXCEPTION (UnsupportedSectorSize); \
	GST_EXCEPTION (VolumeEncryptionNotCompleted); \
	GST_EXCEPTION (VolumeHostInUse); \
	GST_EXCEPTION (VolumeProtected); \
	GST_EXCEPTION (VolumeReadOnly);

	GST_EXCEPTION_SET;

#undef GST_EXCEPTION

}
}

#endif // GST_HEADER_Volume_VolumeExceptions

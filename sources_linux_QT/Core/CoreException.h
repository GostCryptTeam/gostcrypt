/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_CoreException
#define GST_HEADER_Core_CoreException

#include "Platform/Platform.h"

namespace GostCrypt
{
	struct ElevationFailed : public ExecutedProcessFailed
	{
		ElevationFailed () { }
		ElevationFailed (const string &message, const string &command, int exitCode, const string &errorOutput)
			: ExecutedProcessFailed (message, command, exitCode, errorOutput) { }
		GST_SERIALIZABLE_EXCEPTION (ElevationFailed);
	};

	GST_EXCEPTION_DECL (RootDeviceUnavailable, SystemException);

#define GST_EXCEPTION(NAME) GST_EXCEPTION_DECL(NAME,Exception)

#undef GST_EXCEPTION_SET
#define GST_EXCEPTION_SET \
	GST_EXCEPTION_NODECL (ElevationFailed); \
	GST_EXCEPTION_NODECL (RootDeviceUnavailable); \
	GST_EXCEPTION (DriveLetterUnavailable); \
	GST_EXCEPTION (DriverError); \
	GST_EXCEPTION (EncryptedSystemRequired); \
	GST_EXCEPTION (HigherFuseVersionRequired); \
	GST_EXCEPTION (KernelCryptoServiceTestFailed); \
	GST_EXCEPTION (LoopDeviceSetupFailed); \
	GST_EXCEPTION (MountPointRequired); \
	GST_EXCEPTION (MountPointUnavailable); \
	GST_EXCEPTION (NoDriveLetterAvailable); \
	GST_EXCEPTION (TemporaryDirectoryFailure); \
	GST_EXCEPTION (UnsupportedSectorSizeHiddenVolumeProtection); \
	GST_EXCEPTION (UnsupportedSectorSizeNoKernelCrypto); \
	GST_EXCEPTION (VolumeAlreadyMounted); \
	GST_EXCEPTION (VolumeSlotUnavailable);

	GST_EXCEPTION_SET;

#undef GST_EXCEPTION
}

#endif // GST_HEADER_Core_CoreException

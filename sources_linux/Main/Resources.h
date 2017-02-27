/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Resources
#define GST_HEADER_Main_Resources

#include "System.h"
#include "Platform/Platform.h"

namespace GostCrypt
{
	class Resources
	{
	public:
		static string GetLanguageXml ();
		static string GetLegalNotices ();
#ifndef GST_NO_GUI
		static wxBitmap GetDriveIconBitmap ();
		static wxBitmap GetDriveIconMaskBitmap ();
		static wxBitmap GetLogoBitmap ();
		static wxBitmap GetTextualLogoBitmap ();
		static wxIcon GetGostCryptIcon ();
		static wxBitmap GetVolumeCreationWizardBitmap (int height = -1);
#endif
	};
}

#endif // GST_HEADER_Main_Resources

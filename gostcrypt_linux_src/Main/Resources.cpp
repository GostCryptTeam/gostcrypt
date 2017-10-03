/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "System.h"
#include "Platform/Platform.h"
#include "Resources.h"

namespace GostCrypt
{

	wstring Resources::GetLanguageXml ()
	{
		static const wchar_t LanguageXml[] =
		{
#			include "Common/Language.xml.h"
			, 0
		};

		return wstring (LanguageXml);
	}

	wstring Resources::GetLegalNotices ()
	{
		static const wchar_t License[] =
		{
#			include "License.txt.h"
			, 0
		};

		return wstring (License);
	}


#ifndef GST_NO_GUI

	wxBitmap Resources::GetDriveIconBitmap ()
	{
		static const byte DriveIcon[] =
		{
#			include "Mount/Drive_icon_96dpi.bmp.h"
		};

		wxMemoryInputStream stream (DriveIcon, sizeof (DriveIcon));
		return wxBitmap (wxImage (stream).Resize (wxSize (16, 12), wxPoint (0, 0)));
	}

	wxBitmap Resources::GetDriveIconMaskBitmap ()
	{
		static const byte DriveIconMask[] =
		{
#			include "Mount/Drive_icon_mask_96dpi.bmp.h"
		};

		wxMemoryInputStream stream (DriveIconMask, sizeof (DriveIconMask));
		wxImage image (stream);
		image.Resize (wxSize (16, 12), wxPoint (0, 0));

#ifdef __WXGTK__
		return wxBitmap (image.ConvertToMono (0, 0, 0), 1);
#else
		return wxBitmap (image);
#endif
	}


	wxBitmap Resources::GetLogoBitmap ()
	{
		static const byte Logo[] =
		{
#			include "Mount/Logo_96dpi.bmp.h"
		};

		wxMemoryInputStream stream (Logo, sizeof (Logo));
		return wxBitmap (wxImage (stream));
	}

	wxBitmap Resources::GetTextualLogoBitmap ()
	{
		static const byte Logo[] =
		{
#			include "Common/Textual_logo_96dpi.bmp.h"
		};

		wxMemoryInputStream stream (Logo, sizeof (Logo));
		return wxBitmap (wxImage (stream));
	}

	wxIcon Resources::GetGostCryptIcon ()
	{

#		include "Resources/Icons/GostCrypt-16x16.xpm"
		return wxIcon (GostCryptIcon16x16);
	}

	wxBitmap Resources::GetVolumeCreationWizardBitmap (int height)
	{
		static const byte VolumeWizardIcon[] =
		{
#			include "Format/GostCrypt_Wizard.bmp.h"
		};

		wxMemoryInputStream stream (VolumeWizardIcon, sizeof (VolumeWizardIcon));

		wxImage image (stream);
		if (height != -1)
		{
			double scaleFactor = double (height) / double (image.GetHeight());
			image.Rescale (int (image.GetWidth() * scaleFactor), int (image.GetHeight() * scaleFactor), wxIMAGE_QUALITY_HIGH);
		}

		return wxBitmap (image);
	}

#endif // !GST_NO_GUI

}

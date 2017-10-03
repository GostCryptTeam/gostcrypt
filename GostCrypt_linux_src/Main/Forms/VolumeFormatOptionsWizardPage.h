/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_VolumeFormatOptionsWizardPage
#define GST_HEADER_Main_Forms_VolumeFormatOptionsWizardPage

#include "Forms.h"
#include "Core/VolumeCreator.h"

namespace GostCrypt
{
	class VolumeFormatOptionsWizardPage : public VolumeFormatOptionsWizardPageBase
	{
	public:
		VolumeFormatOptionsWizardPage (wxPanel* parent, uint64 volumeSize, uint32 sectorSize, bool enableQuickFormatButton = true, bool disableNoneFilesystem = false, bool disable32bitFilesystems = false);

		VolumeCreationOptions::FilesystemType::Enum GetFilesystemType () const;
		bool IsValid () { return true; }
		bool IsQuickFormatEnabled () const { return QuickFormatCheckBox->IsChecked(); }
		void SetMaxStaticTextWidth (int width) { InfoStaticText->Wrap (width); }
		void SetFilesystemType (VolumeCreationOptions::FilesystemType::Enum type);
		void SetPageText (const wxString &text) { InfoStaticText->SetLabel (text); }
		void SetQuickFormat (bool enabled) { QuickFormatCheckBox->SetValue (enabled); }

	protected:
		void OnFilesystemTypeSelected (wxCommandEvent& event);
		void OnQuickFormatCheckBoxClick (wxCommandEvent& event);
	};
}

#endif // GST_HEADER_Main_Forms_VolumeFormatOptionsWizardPage

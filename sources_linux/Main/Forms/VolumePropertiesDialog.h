/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_VolumePropertiesDialog
#define GST_HEADER_Main_Forms_VolumePropertiesDialog

#include "Forms.h"
#include "Main/Main.h"

namespace GostCrypt
{
	class VolumePropertiesDialog : public VolumePropertiesDialogBase
	{
	public:
		VolumePropertiesDialog (wxWindow* parent, const VolumeInfo &volumeInfo);
		
		void AppendToList (const string &name, const wxString &value);
	};
}

#endif // GST_HEADER_Main_Forms_VolumePropertiesDialog

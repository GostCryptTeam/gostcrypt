/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_AboutDialog
#define GST_HEADER_Main_Forms_AboutDialog

#include "Forms.h"

namespace GostCrypt
{
	class AboutDialog : public AboutDialogBase
	{
	public:
		AboutDialog (wxWindow* parent);
		
		void OnWebsiteHyperlinkClick (wxHyperlinkEvent& event) { Gui->OpenHomepageLink (this, L"main"); }
	};
}

#endif // GST_HEADER_Main_Forms_AboutDialog

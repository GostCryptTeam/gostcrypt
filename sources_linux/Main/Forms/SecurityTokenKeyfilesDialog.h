/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_SecurityTokenKeyfilesDialog
#define GST_HEADER_Main_Forms_SecurityTokenKeyfilesDialog

#include "Forms.h"
#include "Common/SecurityToken.h"
#include "Main/Main.h"

namespace GostCrypt
{
	class SecurityTokenKeyfilesDialog : public SecurityTokenKeyfilesDialogBase
	{
	public:
		SecurityTokenKeyfilesDialog (wxWindow* parent, bool selectionMode = true);
		list <SecurityTokenKeyfilePath> GetSelectedSecurityTokenKeyfilePaths() const { return SelectedSecurityTokenKeyfilePaths; }

	protected:
		enum
		{
			ColumnSecurityTokenSlotId = 0,
			ColumnSecurityTokenLabel,
			ColumnSecurityTokenKeyfileLabel,
		};

		void FillSecurityTokenKeyfileListCtrl ();
		void OnDeleteButtonClick (wxCommandEvent& event);
		void OnExportButtonClick (wxCommandEvent& event);
		void OnImportButtonClick (wxCommandEvent& event);
		void OnListItemActivated (wxListEvent& event) { OnOKButtonClick(); }
		void OnListItemDeselected (wxListEvent& event);
		void OnListItemSelected (wxListEvent& event);
		void OnOKButtonClick ();
		void OnOKButtonClick (wxCommandEvent& event) { OnOKButtonClick(); }

		vector <SecurityTokenKeyfile> SecurityTokenKeyfileList;
		list <SecurityTokenKeyfilePath> SelectedSecurityTokenKeyfilePaths;
	};
}

#endif // GST_HEADER_Main_Forms_SecurityTokenKeyfilesDialog

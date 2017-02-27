/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_PreferencesDialog
#define GST_HEADER_Main_Forms_PreferencesDialog

#include "Forms.h"
#include "Main/Main.h"
#include "KeyfilesPanel.h"

namespace GostCrypt
{
	class PreferencesDialog : public PreferencesDialogBase
	{
	public:
		PreferencesDialog (wxWindow* parent);
		~PreferencesDialog ();

		void SelectPage (wxPanel *page);

	protected:
		void OnBackgroundTaskEnabledCheckBoxClick (wxCommandEvent& event);
		void OnCancelButtonClick (wxCommandEvent& event) { Close(); }
		void OnClose (wxCloseEvent& event);
		void OnDismountOnPowerSavingCheckBoxClick (wxCommandEvent& event);
		void OnDismountOnScreenSaverCheckBoxClick (wxCommandEvent& event);
		void OnForceAutoDismountCheckBoxClick (wxCommandEvent& event);
		void OnOKButtonClick (wxCommandEvent& event);
		void OnPreserveTimestampsCheckBoxClick (wxCommandEvent& event);
		void OnSelectPkcs11ModuleButtonClick (wxCommandEvent& event);


		KeyfilesPanel *DefaultKeyfilesPanel;
		int LastVirtualKeyPressed; 
		unique_ptr <wxTimer> mTimer;
		UserPreferences Preferences;
		bool RestoreValidatorBell;
	};
}

#endif // GST_HEADER_Main_Forms_PreferencesDialog

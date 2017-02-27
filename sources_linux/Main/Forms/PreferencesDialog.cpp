/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "System.h"
#include <wx/dynlib.h>
#include "Common/SecurityToken.h"
#include "Main/Main.h"
#include "Main/Application.h"
#include "Main/GraphicUserInterface.h"
#include "Volume/Cipher.h"
#include "PreferencesDialog.h"

namespace GostCrypt
{
	PreferencesDialog::PreferencesDialog (wxWindow* parent)
		: PreferencesDialogBase (parent),
		LastVirtualKeyPressed (0),
		Preferences (Gui->GetPreferences()),
		RestoreValidatorBell (false)
	{
#define GST_CHECK_BOX_VALIDATOR(NAME) (GST_JOIN(NAME,CheckBox))->SetValidator (wxGenericValidator (&Preferences.NAME));

#ifdef GST_MACOSX
		PreferencesNotebook->SetMinSize (wxSize (Gui->GetCharWidth (PreferencesNotebook) * 108, -1));
#endif
		// Security
		GST_CHECK_BOX_VALIDATOR (DismountOnLogOff);
		GST_CHECK_BOX_VALIDATOR (DismountOnPowerSaving);
		GST_CHECK_BOX_VALIDATOR (DismountOnScreenSaver);
		GST_CHECK_BOX_VALIDATOR (DismountOnInactivity);
		DismountOnInactivitySpinCtrl->SetValidator (wxGenericValidator (&Preferences.MaxVolumeIdleTime));
		GST_CHECK_BOX_VALIDATOR (ForceAutoDismount);
		PreserveTimestampsCheckBox->SetValidator (wxGenericValidator (&Preferences.DefaultMountOptions.PreserveTimestamps));
		GST_CHECK_BOX_VALIDATOR (WipeCacheOnAutoDismount);
		GST_CHECK_BOX_VALIDATOR (WipeCacheOnClose);

		// Mount options
		CachePasswordsCheckBox->SetValidator (wxGenericValidator (&Preferences.DefaultMountOptions.CachePassword));
		MountReadOnlyCheckBox->SetValue (Preferences.DefaultMountOptions.Protection == VolumeProtection::ReadOnly);
		MountRemovableCheckBox->SetValidator (wxGenericValidator (&Preferences.DefaultMountOptions.Removable));

		FilesystemOptionsTextCtrl->SetValue (Preferences.DefaultMountOptions.FilesystemOptions);

		// Keyfiles
		GST_CHECK_BOX_VALIDATOR (UseKeyfiles);

		DefaultKeyfilesPanel = new KeyfilesPanel (DefaultKeyfilesPage, make_shared <KeyfileList> (Preferences.DefaultKeyfiles));
		DefaultKeyfilesSizer->Add (DefaultKeyfilesPanel, 1, wxALL | wxEXPAND);
		DefaultKeyfilesSizer->Layout();

		GST_CHECK_BOX_VALIDATOR (BackgroundTaskEnabled);
		GST_CHECK_BOX_VALIDATOR (CloseBackgroundTaskOnNoVolumes);
		CloseBackgroundTaskOnNoVolumesCheckBox->Show (!Core->IsInPortableMode());
		GST_CHECK_BOX_VALIDATOR (BackgroundTaskMenuDismountItemsEnabled);
		GST_CHECK_BOX_VALIDATOR (BackgroundTaskMenuMountItemsEnabled);
		GST_CHECK_BOX_VALIDATOR (BackgroundTaskMenuOpenItemsEnabled);

		// Security tokens
		Pkcs11ModulePathTextCtrl->SetValue (wstring (Preferences.SecurityTokenModule));
		GST_CHECK_BOX_VALIDATOR (CloseSecurityTokenSessionsAfterMount);

		// System integration
		GST_CHECK_BOX_VALIDATOR (StartOnLogon);
		GST_CHECK_BOX_VALIDATOR (MountDevicesOnLogon);
		GST_CHECK_BOX_VALIDATOR (MountFavoritesOnLogon);

		GST_CHECK_BOX_VALIDATOR (CloseExplorerWindowsOnDismount);
		GST_CHECK_BOX_VALIDATOR (OpenExplorerWindowAfterMount);

		TransferDataToWindow();		// Code below relies on TransferDataToWindow() called at this point

#if defined (GST_MACOSX)
		FilesystemSizer->Show (false);
#else
		// Auto-dismount is not supported on Linux as dismount may require the user to enter admin password
		AutoDismountSizer->Show (false);
		WipeCacheOnAutoDismountCheckBox->Show (false);
#endif

		LogOnSizer->Show (false);
		MountRemovableCheckBox->Show (false);
		CloseExplorerWindowsOnDismountCheckBox->Show (false);

#ifndef wxHAS_POWER_EVENTS
		DismountOnPowerSavingCheckBox->Show (false);
#endif

#ifdef GST_MACOSX
		DismountOnScreenSaverCheckBox->Show (false);
		DismountOnLogOffCheckBox->SetLabel (_("GostCrypt quits"));
		OpenExplorerWindowAfterMountCheckBox->SetLabel (_("Open Finder window for successfully mounted volume"));

		MountRemovableCheckBox->Show (false);
		FilesystemSizer->Show (false);
		LogOnSizer->Show (false);
		CloseExplorerWindowsOnDismountCheckBox->Show (false);
#endif

#ifndef GST_LINUX
		KernelServicesSizer->Show (false);
#endif

		// Page setup
		for (size_t page = 0; page < PreferencesNotebook->GetPageCount(); page++)
		{
			wxNotebookPage *np = PreferencesNotebook->GetPage (page);
			np->Layout();
		}

		Layout();
		Fit();
		Center();

		StdButtonsOK->SetDefault();
	}

	PreferencesDialog::~PreferencesDialog ()
	{
		if (RestoreValidatorBell)
			wxTextValidator::SetBellOnError (false);
	}

	void PreferencesDialog::SelectPage (wxPanel *page)
	{
		for (size_t pageIndex = 0; pageIndex < PreferencesNotebook->GetPageCount(); pageIndex++)
		{
			if (PreferencesNotebook->GetPage (pageIndex) == page)
				PreferencesNotebook->ChangeSelection (pageIndex);
		}
	}

	void PreferencesDialog::OnBackgroundTaskEnabledCheckBoxClick (wxCommandEvent& event)
	{
		if (!event.IsChecked())
			BackgroundTaskEnabledCheckBox->SetValue (!Gui->AskYesNo (LangString["CONFIRM_BACKGROUND_TASK_DISABLED"], false, true));
	}

	void PreferencesDialog::OnClose (wxCloseEvent& event)
	{
		event.Skip();
	}

	void PreferencesDialog::OnDismountOnPowerSavingCheckBoxClick (wxCommandEvent& event)
	{
		if (event.IsChecked() && !ForceAutoDismountCheckBox->IsChecked())
			Gui->ShowWarning ("WARN_PREF_AUTO_DISMOUNT");
	}

	void PreferencesDialog::OnDismountOnScreenSaverCheckBoxClick (wxCommandEvent& event)
	{
		if (event.IsChecked() && !ForceAutoDismountCheckBox->IsChecked())
			Gui->ShowWarning ("WARN_PREF_AUTO_DISMOUNT");
	}

	void PreferencesDialog::OnForceAutoDismountCheckBoxClick (wxCommandEvent& event)
	{
		if (!event.IsChecked())
			ForceAutoDismountCheckBox->SetValue (!Gui->AskYesNo (LangString["CONFIRM_NO_FORCED_AUTODISMOUNT"], false, true));
	}

	void PreferencesDialog::OnOKButtonClick (wxCommandEvent& event)
	{
		if (!Validate())
			return;

		TransferDataFromWindow();

		Preferences.DefaultMountOptions.Protection = MountReadOnlyCheckBox->IsChecked() ? VolumeProtection::ReadOnly : VolumeProtection::None;
		Preferences.DefaultMountOptions.FilesystemOptions = FilesystemOptionsTextCtrl->GetValue();
		Preferences.DefaultKeyfiles = *DefaultKeyfilesPanel->GetKeyfiles();

		bool securityTokenModuleChanged = (Preferences.SecurityTokenModule != wstring (Pkcs11ModulePathTextCtrl->GetValue()));
		Preferences.SecurityTokenModule = wstring (Pkcs11ModulePathTextCtrl->GetValue());

		Gui->SetPreferences (Preferences);

		try
		{
			if (securityTokenModuleChanged)
			{
				if (Preferences.SecurityTokenModule.IsEmpty())
				{
					if (SecurityToken::IsInitialized())
						SecurityToken::CloseLibrary ();
				}
				else
				{
					Gui->InitSecurityTokenLibrary(); 
				}
			}
		}
		catch (exception &e)
		{
			Gui->ShowError (e);
		}

		EndModal (wxID_OK);
	}
	
	void PreferencesDialog::OnPreserveTimestampsCheckBoxClick (wxCommandEvent& event)
	{
#ifdef GST_LINUX
		if (!event.IsChecked())
			Gui->ShowInfo (_("Please note that disabling this option may have no effect on volumes mounted using kernel cryptographic services."));
#endif
	}

	void PreferencesDialog::OnSelectPkcs11ModuleButtonClick (wxCommandEvent& event)
	{
		list < pair <wstring, wstring> > extensions;
		wxString libExtension;
		libExtension = wxDynamicLibrary::CanonicalizeName (L"x");

#ifdef GST_MACOSX
		extensions.push_back (make_pair (L"dylib", LangString["DLL_FILES"].ToStdWstring()));
#endif
		if (!libExtension.empty())
		{
			extensions.push_back (make_pair (libExtension.Mid (libExtension.find (L'.') + 1).ToStdWstring(), LangString["DLL_FILES"].ToStdWstring()));
			extensions.push_back (make_pair (L"*", L""));
		}

		string libDir;

#ifdef GST_MACOSX
		libDir = "/usr/local/lib";
#elif defined (GST_UNIX)
		libDir = "/usr/lib";
#endif

		Gui->ShowInfo ("SELECT_PKCS11_MODULE_HELP");

		FilePathList files = Gui->SelectFiles (this, LangString["SELECT_PKCS11_MODULE"], false, false, extensions, libDir);
		if (!files.empty())
			Pkcs11ModulePathTextCtrl->SetValue (wstring (*files.front()));
	}

}

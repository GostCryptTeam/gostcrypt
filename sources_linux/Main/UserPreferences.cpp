/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "System.h"
#include "Main/Application.h"
#include "UserPreferences.h"
#include "Xml.h"

namespace GostCrypt
{
	void UserPreferences::SetValue (const wxString &cfgText, bool &cfgVar)
	{
		if (cfgText == L"0")
			cfgVar = false;
		else if (cfgText == L"1")
			cfgVar = true;
	}

	void UserPreferences::SetValue (const wxString &cfgText, int &cfgVar)
	{
		if (cfgText.empty())
			cfgVar = 0;
		else
			cfgVar = StringConverter::ToUInt32 (wstring (cfgText));
	}
	
	void UserPreferences::SetValue (const wxString &cfgText, uint64 &cfgVar)
	{
		if (cfgText.empty())
			cfgVar = 0;
		else
			cfgVar = StringConverter::ToUInt64 (wstring (cfgText));
	}

	void UserPreferences::SetValue (const wxString &cfgText, wstring &cfgVar)
	{
		cfgVar = cfgText;
	}

	void UserPreferences::SetValue (const wxString &cfgText, wxString &cfgVar)
	{
		cfgVar = cfgText;
	}

	void UserPreferences::SetValue (const wxString &cfgText, FilesystemPath &cfgVar)
	{
		cfgVar = wstring (cfgText);
	}

	void UserPreferences::Load()
	{
		// Preferences
		FilePath cfgPath = Application::GetConfigFilePath (GetPreferencesFileName());
		if (cfgPath.IsFile())
		{
			map <wxString, wxString> configMap;
			foreach (XmlNode node, XmlParser (cfgPath).GetNodes (L"config"))
			{
				configMap[node.Attributes[L"key"]] = node.InnerText;
			}

#define GST_CONFIG_SET(NAME) SetValue (configMap[L###NAME], NAME)

			GST_CONFIG_SET (BackgroundTaskEnabled);
			GST_CONFIG_SET (BackgroundTaskMenuDismountItemsEnabled);
			GST_CONFIG_SET (BackgroundTaskMenuMountItemsEnabled);
			GST_CONFIG_SET (BackgroundTaskMenuOpenItemsEnabled);

			SetValue (configMap[L"CachePasswords"], DefaultMountOptions.CachePassword);
			GST_CONFIG_SET (CloseBackgroundTaskOnNoVolumes);
			GST_CONFIG_SET (CloseExplorerWindowsOnDismount);
			GST_CONFIG_SET (CloseSecurityTokenSessionsAfterMount);
			GST_CONFIG_SET (DisableKernelEncryptionModeWarning);
			GST_CONFIG_SET (DismountOnInactivity);
			GST_CONFIG_SET (DismountOnLogOff);
			GST_CONFIG_SET (DismountOnPowerSaving);
			GST_CONFIG_SET (DismountOnScreenSaver);

			GST_CONFIG_SET (BackgroundTaskEnabled);
			SetValue (configMap[L"FilesystemOptions"], DefaultMountOptions.FilesystemOptions);
			GST_CONFIG_SET (ForceAutoDismount);
			GST_CONFIG_SET (LastSelectedSlotNumber);
			GST_CONFIG_SET (MaxVolumeIdleTime);
			GST_CONFIG_SET (MountDevicesOnLogon);
			GST_CONFIG_SET (MountFavoritesOnLogon);

			bool readOnly;
			SetValue (configMap[L"MountVolumesReadOnly"], readOnly);
			DefaultMountOptions.Protection = readOnly ? VolumeProtection::ReadOnly : VolumeProtection::None;

			SetValue (configMap[L"MountVolumesRemovable"], DefaultMountOptions.Removable);
			SetValue (configMap[L"NoHardwareCrypto"], DefaultMountOptions.NoHardwareCrypto);
			SetValue (configMap[L"NoKernelCrypto"], DefaultMountOptions.NoKernelCrypto);
			GST_CONFIG_SET (OpenExplorerWindowAfterMount);
			SetValue (configMap[L"PreserveTimestamps"], DefaultMountOptions.PreserveTimestamps);
			GST_CONFIG_SET (SaveHistory);
			SetValue (configMap[L"SecurityTokenLibrary"], SecurityTokenModule);
			GST_CONFIG_SET (StartOnLogon);
			GST_CONFIG_SET (UseKeyfiles);
			GST_CONFIG_SET (WipeCacheOnAutoDismount);
			GST_CONFIG_SET (WipeCacheOnClose);
		}

		// Default keyfiles
		cfgPath = Application::GetConfigFilePath (GetDefaultKeyfilesFileName());
		if (cfgPath.IsFile())
		{
			foreach (const XmlNode &node, XmlParser (cfgPath).GetNodes (L"keyfile"))
			{
				DefaultKeyfiles.push_back (make_shared <Keyfile> ((wstring) node.InnerText));
			}
		}
	}

	void UserPreferences::Save() const
	{
		// Preferences
		class ConfigXmlFormatter
		{
		public:
			void AddEntry (const wchar_t *key, const wxString &value)
			{
				if (!value.empty())
				{
					XmlNode config (L"config");
					config.Attributes[L"key"] = key;
					config.InnerText = value;
					XmlConfig.InnerNodes.push_back (config);
				}
			}

			void AddEntry (const wchar_t *key, bool value)
			{
				AddEntry (key, wxString (value ? L"1" : L"0"));
			}

			void AddEntry (const wchar_t *key, int value)
			{
				wstringstream s;
				s << value;
				AddEntry (key, s.str());
			}

			void AddEntry (const wchar_t *key, uint64 value)
			{
				wstringstream s;
				s << value;
				AddEntry (key, s.str());
			}

			XmlNode XmlConfig;
		};

		ConfigXmlFormatter formatter;
		formatter.XmlConfig.Name = L"configuration";

#define GST_CONFIG_ADD(NAME) formatter.AddEntry (L###NAME, NAME)

		GST_CONFIG_ADD (BackgroundTaskEnabled);
		GST_CONFIG_ADD (BackgroundTaskMenuDismountItemsEnabled);
		GST_CONFIG_ADD (BackgroundTaskMenuMountItemsEnabled);
		GST_CONFIG_ADD (BackgroundTaskMenuOpenItemsEnabled);
		formatter.AddEntry (L"CachePasswords", DefaultMountOptions.CachePassword);
		GST_CONFIG_ADD (CloseBackgroundTaskOnNoVolumes);
		GST_CONFIG_ADD (CloseExplorerWindowsOnDismount);
		GST_CONFIG_ADD (CloseSecurityTokenSessionsAfterMount);
		GST_CONFIG_ADD (DisableKernelEncryptionModeWarning);
		GST_CONFIG_ADD (DismountOnInactivity);
		GST_CONFIG_ADD (DismountOnLogOff);
		GST_CONFIG_ADD (DismountOnPowerSaving);
		GST_CONFIG_ADD (DismountOnScreenSaver);
		GST_CONFIG_ADD (BackgroundTaskEnabled);
		formatter.AddEntry (L"FilesystemOptions", DefaultMountOptions.FilesystemOptions);
		GST_CONFIG_ADD (ForceAutoDismount);
		GST_CONFIG_ADD (LastSelectedSlotNumber);
		GST_CONFIG_ADD (MaxVolumeIdleTime);
		GST_CONFIG_ADD (MountDevicesOnLogon);
		GST_CONFIG_ADD (MountFavoritesOnLogon);
		formatter.AddEntry (L"MountVolumesReadOnly", DefaultMountOptions.Protection == VolumeProtection::ReadOnly);
		formatter.AddEntry (L"MountVolumesRemovable", DefaultMountOptions.Removable);
		formatter.AddEntry (L"NoHardwareCrypto", DefaultMountOptions.NoHardwareCrypto);
		formatter.AddEntry (L"NoKernelCrypto", DefaultMountOptions.NoKernelCrypto);
		GST_CONFIG_ADD (OpenExplorerWindowAfterMount);
		formatter.AddEntry (L"PreserveTimestamps", DefaultMountOptions.PreserveTimestamps);
		GST_CONFIG_ADD (SaveHistory);
		formatter.AddEntry (L"SecurityTokenLibrary", wstring (SecurityTokenModule));
		GST_CONFIG_ADD (StartOnLogon);
		GST_CONFIG_ADD (UseKeyfiles);
		GST_CONFIG_ADD (WipeCacheOnAutoDismount);
		GST_CONFIG_ADD (WipeCacheOnClose);

		XmlWriter writer (Application::GetConfigFilePath (GetPreferencesFileName(), true));
		writer.WriteNode (formatter.XmlConfig);
		writer.Close();

		// Default keyfiles
		FilePath keyfilesCfgPath = Application::GetConfigFilePath (GetDefaultKeyfilesFileName(), true);
		
		if (DefaultKeyfiles.empty())
		{
			if (keyfilesCfgPath.IsFile())
				keyfilesCfgPath.Delete();
		}
		else
		{
			XmlNode keyfilesXml (L"defaultkeyfiles");

			foreach_ref (const Keyfile &keyfile, DefaultKeyfiles)
			{
				keyfilesXml.InnerNodes.push_back (XmlNode (L"keyfile", wxString (wstring(FilesystemPath (keyfile)))));
			}

			XmlWriter keyfileWriter (keyfilesCfgPath);
			keyfileWriter.WriteNode (keyfilesXml);
			keyfileWriter.Close();
		}
	}
}

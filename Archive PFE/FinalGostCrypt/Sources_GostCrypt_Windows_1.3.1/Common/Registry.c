/**
 *
 *	\file Registry.c
 *	\brief Manage the registry values for windows
 *	\version 1.3.1
 *	\date 20/02/2017
 *
 */

/**
 *
 *	Copyright (c) 2004-2010 TrueCrypt Developers Association. All rights reserved.
 *
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include "Gstdefs.h"
#include "Registry.h"

/**
 *
 *	\fn BOOL ReadLocalMachineRegistryDword (char *subKey, char *name, DWORD *value)
 *	\brief Query the registery and read it
 *	\param[in] DWORD * value
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return BOOL FALSE if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
 *			FALSE if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) value, &size) != ERROR_SUCCESS)
 *			type == REG_DWORD else
 */
BOOL ReadLocalMachineRegistryDword (char *subKey, char *name, DWORD *value)
{
	HKEY hkey = 0;
	DWORD size = sizeof (*value);
	DWORD type;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) value, &size) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		return FALSE;
	}

	RegCloseKey (hkey);
	return type == REG_DWORD;
}

/**
 *
 *	\fn BOOL ReadLocalMachineRegistryMultiString (char *subKey, char *name, char *value, DWORD *size)
 *	\brief Query the registry and read multiple strings, ony the size of the data read changes with ReadLocalMachineRegistryDword.
 *	\param[in] DWORD * size
 *	\param[in] char * value
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return BOOL FALSE if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
 *			FALSE if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) value, size) != ERROR_SUCCESS)
 *			(type == REG_MULTI_SZ) else
 *
 */
BOOL ReadLocalMachineRegistryMultiString (char *subKey, char *name, char *value, DWORD *size)
{
	HKEY hkey = 0;
	DWORD type;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) value, size) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		return FALSE;
	}

	RegCloseKey (hkey);
	return type == REG_MULTI_SZ;
}

/**
 *
 *	\fn BOOL ReadLocalMachineRegistryString (const char *subKey, char *name, char *str, DWORD *size)
 *	\brief Query the registry and read a string, ony the size of the data read changes with ReadLocalMachineRegistryDword.
 *	\param[in] DWORD * size
 *	\param[in] char * str
 *	\param[in] char * name
 *	\param[in] const char * subKey
 *	\return BOOL FALSE if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
 *			FALSE if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) str, size) != ERROR_SUCCESS)
 *			(type == REG_SZ) else
 *
 */
BOOL ReadLocalMachineRegistryString (const char *subKey, char *name, char *str, DWORD *size)
{
	HKEY hkey = 0;
	DWORD type;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) str, size) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		return FALSE;
	}

	RegCloseKey (hkey);
	return type == REG_SZ;
}

/**
 *
 *	\fn BOOL ReadLocalMachineRegistryStringNonReflected (const char *subKey, char *name, char *str, DWORD *size)
 *	\brief Read a string in the registry in non reflected mode.
 *	\param[in] DWORD * size
 *	\param[in] char * str
 *	\param[in] char * name
 *	\param[in] const char * subKey
 *	\return BOOL FALSE if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ | KEY_WOW64_64KEY, &hkey) != ERROR_SUCCESS)
 *			FALSE if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) str, size) != ERROR_SUCCESS)
 *			(type == REG_SZ) else
 *
 */
BOOL ReadLocalMachineRegistryStringNonReflected (const char *subKey, char *name, char *str, DWORD *size)
{
	HKEY hkey = 0;
	DWORD type;

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ | KEY_WOW64_64KEY, &hkey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx (hkey, name, NULL, &type, (BYTE *) str, size) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		return FALSE;
	}

	RegCloseKey (hkey);
	return type == REG_SZ;
}

/**
 *
 *	\fn int ReadRegistryInt (char *subKey, char *name, int defaultValue)
 *	\brief Read a int value in the registry and return it !
 *	\param[in] int defaultValue
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return int value
 *
 */
int ReadRegistryInt (char *subKey, char *name, int defaultValue)
{
	HKEY hkey = 0;
	DWORD value, size = sizeof (DWORD);

	if (RegOpenKeyEx (HKEY_CURRENT_USER, subKey,
		0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return defaultValue;

	if (RegQueryValueEx (hkey, name, 0,	0, (LPBYTE) &value, &size) != ERROR_SUCCESS)
		value = defaultValue;

	RegCloseKey (hkey);
	return value;
}

/**
 *
 *	\fn char * ReadRegistryString (char *subKey, char *name, char *defaultValue, char *str, int maxLen)
 *	\brief Read in the registry a string and return it
 *	\param[in] int maxLen 
 *	\param[in] char * str
 *	\param[in] char * defaultValue
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return char * str
 *
 */
char *ReadRegistryString (char *subKey, char *name, char *defaultValue, char *str, int maxLen)
{
	HKEY hkey = 0;
	char value[MAX_PATH*4];
	DWORD size = sizeof (value);

	strncpy (str, defaultValue, maxLen-1);

	ZeroMemory (value, sizeof value);
	if (RegOpenKeyEx (HKEY_CURRENT_USER, subKey,
		0, KEY_READ, &hkey) == ERROR_SUCCESS)
		if (RegQueryValueEx (hkey, name, 0,	0, (LPBYTE) &value,	&size) == ERROR_SUCCESS)
			strncpy (str, value, maxLen-1);

	RegCloseKey (hkey);
	return str;
}

/**
 *
 *	\fn DWORD ReadRegistryBytes (char *path, char *name, char *value, int maxLen)
 *	\brief Read bytes in the registry
 *	\param[in] int maxLen
 *	\param[in] char * value
 *	\param[in] char * name
 *	\param[in] char * path
 *	\return DWORD 0 if (RegOpenKeyEx (HKEY_CURRENT_USER, path, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
 *			(success ? size : 0) else
 *	
 */
DWORD ReadRegistryBytes (char *path, char *name, char *value, int maxLen)
{
	HKEY hkey = 0;
	DWORD size = maxLen;
	BOOL success = FALSE;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, path, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return 0;

	success = (RegQueryValueEx (hkey, name, 0,	0, (LPBYTE) value,	&size) == ERROR_SUCCESS);
	RegCloseKey (hkey);

	return success ? size : 0;
}

/**
 *
 *	\fn void WriteRegistryInt (char *subKey, char *name, int value)
 *	\brief Write a int in the registry
 *	\param[in] int value
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return void
 *
 */
void WriteRegistryInt (char *subKey, char *name, int value)
{
	HKEY hkey = 0;
	DWORD disp;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp) != ERROR_SUCCESS)
		return;

	RegSetValueEx (hkey, name, 0, REG_DWORD, (BYTE *) &value, sizeof value);
	RegCloseKey (hkey);
}

/**
 *
 *	\fn BOOL WriteLocalMachineRegistryDword (char *subKey, char *name, DWORD value)
 *	\brief Write a Dword in the local machine registry.
 *	\param[in] DWORD value
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return BOOL FALSE if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
 *			FALSE if ((status = RegSetValueEx (hkey, name, 0, REG_DWORD, (BYTE *) &value, sizeof value)) != ERROR_SUCCESS)
 *			TRUE else
 *
 */
BOOL WriteLocalMachineRegistryDword (char *subKey, char *name, DWORD value)
{
	HKEY hkey = 0;
	DWORD disp;
	LONG status;

	if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
	{
		SetLastError (status);
		return FALSE;
	}

	if ((status = RegSetValueEx (hkey, name, 0, REG_DWORD, (BYTE *) &value, sizeof value)) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		SetLastError (status);
		return FALSE;
	}

	RegCloseKey (hkey);
	return TRUE;
}

/**
 *
 *	\fn BOOL WriteLocalMachineRegistryMultiString (char *subKey, char *name, char *multiString, DWORD size)
 *	\brief Write multiple lines in the local machine registry.
 *	\param[in] DWORD size
 *	\param[in] char * multiString
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return BOOL FALSE if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
 *			FALSE if ((status = RegSetValueEx (hkey, name, 0, REG_MULTI_SZ, (BYTE *) multiString, size)) != ERROR_SUCCESS)
 *			TRUE else
 *
 */
BOOL WriteLocalMachineRegistryMultiString (char *subKey, char *name, char *multiString, DWORD size)
{
	HKEY hkey = 0;
	DWORD disp;
	LONG status;

	if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
	{
		SetLastError (status);
		return FALSE;
	}

	if ((status = RegSetValueEx (hkey, name, 0, REG_MULTI_SZ, (BYTE *) multiString, size)) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		SetLastError (status);
		return FALSE;
	}

	RegCloseKey (hkey);
	return TRUE;
}

/**
 *
 *	\fn BOOL WriteLocalMachineRegistryString (char *subKey, char *name, char *str, BOOL expandable)
 *	\brief Write a whole string in the local machine resgistry
 *	\param[in] BOOL expandable
 *	\param[in] char * str
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return BOOL FALSE if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
 *			FALSE if ((status = RegSetValueEx (hkey, name, 0, expandable ? REG_EXPAND_SZ : REG_SZ, (BYTE *) str, strlen (str) + 1)) != ERROR_SUCCESS)
 *			TRUE else
 *
 */
BOOL WriteLocalMachineRegistryString (char *subKey, char *name, char *str, BOOL expandable)
{
	HKEY hkey = 0;
	DWORD disp;
	LONG status;

	if ((status = RegCreateKeyEx (HKEY_LOCAL_MACHINE, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp)) != ERROR_SUCCESS)
	{
		SetLastError (status);
		return FALSE;
	}

	if ((status = RegSetValueEx (hkey, name, 0, expandable ? REG_EXPAND_SZ : REG_SZ, (BYTE *) str, strlen (str) + 1)) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		SetLastError (status);
		return FALSE;
	}

	RegCloseKey (hkey);
	return TRUE;
}

/**
 *
 *	\fn void WriteRegistryString (char *subKey, char *name, char *str)
 *	\brief Write a string in the registry
 *	\param[in] char * str
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return void
 *
 */
void WriteRegistryString (char *subKey, char *name, char *str)
{
	HKEY hkey = 0;
	DWORD disp;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp) != ERROR_SUCCESS)
		return;

	RegSetValueEx (hkey, name, 0, REG_SZ, (BYTE *) str, strlen (str) + 1);
	RegCloseKey (hkey);
}

/**
 *
 *	\fn BOOL WriteRegistryBytes (char *path, char *name, char *str, DWORD size)
 *	\brief Write bytes in the registry
 *	\param[in] DWORD size
 *	\param[in] char * str
 *	\param[in] char * name
 *	\param[in] char * path
 *	\return BOOL FALSE if (RegCreateKeyEx (HKEY_CURRENT_USER, path,0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp) != ERROR_SUCCESS)
 *			res == ERROR_SUCCESS else
 *
 */
BOOL WriteRegistryBytes (char *path, char *name, char *str, DWORD size)
{
	HKEY hkey = 0;
	DWORD disp;
	BOOL res;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, path,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &disp) != ERROR_SUCCESS)
		return FALSE;

	res = RegSetValueEx (hkey, name, 0, REG_BINARY, (BYTE *) str, size);
	RegCloseKey (hkey);
	return res == ERROR_SUCCESS;
}

/**
 *
 *	\fn BOOL DeleteLocalMachineRegistryKey (char *parentKey, char *subKeyToDelete)
 *	\brief Delete the local machine registry key
 *	\param[in] char * subKeyToDelete
 *	\param[in] char * parentKey
 *	\return BOOL FALSE if ((status = RegOpenKeyEx (HKEY_LOCAL_MACHINE, parentKey, 0, KEY_WRITE, &hkey)) != ERROR_SUCCESS)
 *			FALSE if ((status = RegDeleteKey (hkey, subKeyToDelete)) != ERROR_SUCCESS)
 *			TRUE else
 *
 */
BOOL DeleteLocalMachineRegistryKey (char *parentKey, char *subKeyToDelete)
{
	LONG status;
	HKEY hkey = 0;

	if ((status = RegOpenKeyEx (HKEY_LOCAL_MACHINE, parentKey, 0, KEY_WRITE, &hkey)) != ERROR_SUCCESS)
	{
		SetLastError (status);
		return FALSE;
	}

	if ((status = RegDeleteKey (hkey, subKeyToDelete)) != ERROR_SUCCESS)
	{
		RegCloseKey (hkey);
		SetLastError (status);
		return FALSE;
	}

	RegCloseKey (hkey);
	return TRUE;
}

/**
 *
 *	\fn void DeleteRegistryValue (char *subKey, char *name)
 *	\brief Delete a registry value
 *	\param[in] char * name
 *	\param[in] char * subKey
 *	\return void
 *
 */
void DeleteRegistryValue (char *subKey, char *name)
{
	HKEY hkey = 0;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hkey) != ERROR_SUCCESS)
		return;

	RegDeleteValue (hkey, name);
	RegCloseKey (hkey);
}


/**
 *
 *	\fn void GetStartupRegKeyName (char *regk)
 *	\brief Getter for the registry key name
 *	\param[in] char * regk
 *	\return void
 *
 */
void GetStartupRegKeyName (char *regk)
{
	// The string is split in order to prevent some antivirus packages from falsely reporting  
	// GostCrypt.exe to contain a possible Trojan horse because of this string (heuristic scan).
	sprintf (regk, "%s%s", "Software\\Microsoft\\Windows\\Curren", "tVersion\\Run");
}

/**
 *
 *	\file Dir.c
 *	\brief Manage the installation directory
 *	\version 1.3.1
 *	\date 14/13/2016
 *
 */

/**
 *	
 *	Legal Notice: Some portions of the source code contained in this file were
 *	derived from the source code of Encryption for the Masses 2.02a, which is
 *	Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 *	Agreement for Encryption for the Masses'. Modifications and additions to
 *	the original source code (contained in this file) and all other portions
 *	of this file are Copyright (c) 2003-2008 TrueCrypt Developers Association
 *	and are governed by the TrueCrypt License 3.0 the full text of which is
 *	contained in the file License.txt included in TrueCrypt binary and source
 *	code distribution packages. 
 *	
 */

#include "Gstdefs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "Dir.h"

/**
 *
 *	\fn int mkfulldir (char *oriPath, BOOL bCheckonly)
 *	\brief Create full directory tree. returns 0 for success, -1 if failure
 *	\param[in] char * oriPath
 *	\param[in] BOOL bCheckonly
 *	\return int _stat (path, &st) if bCheckonly
 *			_stat (path, &st)) if mkfulldir_internal (path)
 *			0 else
 *
 */
int mkfulldir (char *oriPath, BOOL bCheckonly)
{
	struct _stat st;
	char *uniq_file;
	char path [GST_MAX_PATH];

	strcpy (path, oriPath);

	if (strlen (path) == 3 && path[1] == ':')
		goto is_root;	/* keep final slash in root if present */

	/* strip final forward or backslash if we have one! */
	uniq_file = strrchr (path, '\\');
	if (uniq_file && uniq_file[1] == '\0')
		uniq_file[0] = '\0';
	else
	{
		uniq_file = strrchr (path, '/');
		if (uniq_file && uniq_file[1] == '\0')
			uniq_file[0] = '\0';
	}

      is_root:
	if (bCheckonly)
		return _stat (path, &st);

	if (_stat (path, &st))
		return mkfulldir_internal (path);
	else
		return 0;
}


/**
 *
 *	\fn int mkfulldir_internal (char *path)
 *	\brief Set the directory in the program for the drives.
 *	\param[in] char * path
 *	\return int _stat (path, &st)
 *
 */
int mkfulldir_internal (char *path)
{
	char *token;
	struct _stat st;
	static char tokpath[_MAX_PATH];
	static char trail[_MAX_PATH];

	strcpy (tokpath, path);
	trail[0] = '\0';

	token = strtok (tokpath, "\\/");

	if (tokpath[0] == '\\' && tokpath[1] == '\\')
	{			/* unc */
		trail[0] = tokpath[0];
		trail[1] = tokpath[1];
		trail[2] = '\0';
		strcat (trail, token);
		strcat (trail, "\\");
		token = strtok (NULL, "\\/");
		if (token)
		{		/* get share name */
			strcat (trail, token);
			strcat (trail, "\\");
		}
		token = strtok (NULL, "\\/");
	}

	if (tokpath[1] == ':')
	{			/* drive letter */
		strcat (trail, tokpath);
		strcat (trail, "\\");
		token = strtok (NULL, "\\/");
	}

	while (token != NULL)
	{
		int x;
		strcat (trail, token);
		x = _mkdir (trail);
		strcat (trail, "\\");
		token = strtok (NULL, "\\/");
	}

	return _stat (path, &st);
}

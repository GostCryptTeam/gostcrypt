/*
 * Portable xxxat() functions.
 *
 * No copyright is claimed.  This code is in the public domain; do with
 * it what you wish.
 *
 * Written by Karel Zak <kzak@redhat.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "at.h"
#include "c.h"

#ifdef HAVE_FSTATAT
int fstat_at(int dir, const char *dirname __attribute__ ((__unused__)),
	     const char *filename, struct stat *st, int nofollow)
{
	return fstatat(dir, filename, st,
			nofollow ? AT_SYMLINK_NOFOLLOW : 0);
}
#else
int fstat_at(int dir, const char *dirname, const char *filename,
				struct stat *st, int nofollow)
{
	(void)dir;
	if (*filename != '/') {
		char path[PATH_MAX];
		int len;

		len = snprintf(path, sizeof(path), "%s/%s", dirname, filename);
        if ((len < 0) || (((unsigned int)len + 1) > sizeof(path)))
			return -1;

		return nofollow ? lstat(path, st) : stat(path, st);
	}

	return nofollow ? lstat(filename, st) : stat(filename, st);
}
#endif

#ifdef HAVE_FSTATAT
int open_at(int dir, const char *dirname __attribute__ ((__unused__)),
	    const char *filename, int flags)
{
	return openat(dir, filename, flags);
}
#else
int open_at(int dir, const char *dirname, const char *filename, int flags)
{
	(void)dir;
	if (*filename != '/') {
		char path[PATH_MAX];
		int len;

		len = snprintf(path, sizeof(path), "%s/%s", dirname, filename);
        if ((len < 0) || (((unsigned int)len + 1) > sizeof(path)))
			return -1;

		return open(path, flags);
	}
	return open(filename, flags);
}
#endif

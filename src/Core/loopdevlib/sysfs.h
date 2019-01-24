/*
 * Copyright (C) 2011 Karel Zak <kzak@redhat.com>
 */
#ifndef UTIL_LINUX_SYSFS_H
#define UTIL_LINUX_SYSFS_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>

struct sysfs_cxt
{
    dev_t   devno;
    int dir_fd;     /* /sys/block/<name> */
    char*    dir_path;
    struct sysfs_cxt* parent;

    unsigned int    scsi_host,
             scsi_channel,
             scsi_target,
             scsi_lun;

    unsigned int    has_hctl : 1;
};

#define UL_SYSFSCXT_EMPTY { 0, -1, NULL, NULL, 0, 0, 0, 0, 0 }

extern char* sysfs_devno_attribute_path(dev_t devno, char* buf,
                                        size_t bufsiz, const char* attr);
extern char* sysfs_devno_path(dev_t devno, char* buf, size_t bufsiz);
extern dev_t sysfs_devname_to_devno(const char* name, const char* parent);

extern int sysfs_init(struct sysfs_cxt* cxt, dev_t devno, struct sysfs_cxt* parent)
__attribute__((warn_unused_result));
extern void sysfs_deinit(struct sysfs_cxt* cxt);

extern int sysfs_scanf(struct sysfs_cxt* cxt, const char* attr,
                       const char* fmt, ...)
__attribute__((format(scanf, 3, 4)));

extern int sysfs_read_u64(struct sysfs_cxt* cxt, const char* attr, uint64_t* res);

#endif /* UTIL_LINUX_SYSFS_H */

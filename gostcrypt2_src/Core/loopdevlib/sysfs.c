/*
 * No copyright is claimed.  This code is in the public domain; do with
 * it what you wish.
 *
 * Written by Karel Zak <kzak@redhat.com>
 */
#include <ctype.h>

#include "c.h"
#include "at.h"
#include "pathnames.h"
#include "sysfs.h"

char *sysfs_devno_attribute_path(dev_t devno, char *buf,
				 size_t bufsiz, const char *attr)
{
	int len;

	if (attr)
		len = snprintf(buf, bufsiz, _PATH_SYS_DEVBLOCK "/%d:%d/%s",
			major(devno), minor(devno), attr);
	else
		len = snprintf(buf, bufsiz, _PATH_SYS_DEVBLOCK "/%d:%d",
			major(devno), minor(devno));

	return (len < 0 || (size_t) len + 1 > bufsiz) ? NULL : buf;
}
char *sysfs_devno_path(dev_t devno, char *buf, size_t bufsiz)
{
	return sysfs_devno_attribute_path(devno, buf, bufsiz, NULL);
}

dev_t sysfs_devname_to_devno(const char *name, const char *parent)
{
	char buf[PATH_MAX], *path = NULL;
	dev_t dev = 0;

	if (strncmp("/dev/", name, 5) == 0) {
		/*
		 * Read from /dev
		 */
		struct stat st;

		if (stat(name, &st) == 0)
			dev = st.st_rdev;
		else
			name += 5;	/* unaccesible, or not node in /dev */
	}

	if (!dev && parent && strncmp("dm-", name, 3)) {
		/*
		 * Create path to /sys/block/<parent>/<name>/dev
		 */
		int len = snprintf(buf, sizeof(buf),
				_PATH_SYS_BLOCK "/%s/%s/dev", parent, name);
		if (len < 0 || (size_t) len + 1 > sizeof(buf))
			return 0;
		path = buf;

	} else if (!dev) {
		/*
		 * Create path to /sys/block/<name>/dev
		 */
		int len = snprintf(buf, sizeof(buf),
				_PATH_SYS_BLOCK "/%s/dev", name);
		if (len < 0 || (size_t) len + 1 > sizeof(buf))
			return 0;
		path = buf;
	}

	if (path) {
		/*
		 * read devno from sysfs
		 */
		FILE *f;
		int maj = 0, min = 0;

		f = fopen(path, "r" UL_CLOEXECSTR);
		if (!f)
			return 0;

		if (fscanf(f, "%d:%d", &maj, &min) == 2)
			dev = makedev(maj, min);
		fclose(f);
	}
	return dev;
}

int sysfs_init(struct sysfs_cxt *cxt, dev_t devno, struct sysfs_cxt *parent)
{
	char path[PATH_MAX];
	int fd, rc;

	memset(cxt, 0, sizeof(*cxt));
	cxt->dir_fd = -1;

	if (!sysfs_devno_path(devno, path, sizeof(path)))
		goto err;

	fd = open(path, O_RDONLY|O_CLOEXEC);
	if (fd < 0)
		goto err;
	cxt->dir_fd = fd;

	cxt->dir_path = strdup(path);
	if (!cxt->dir_path)
		goto err;
	cxt->devno = devno;
	cxt->parent = parent;
	return 0;
err:
	rc = errno > 0 ? -errno : -1;
	sysfs_deinit(cxt);
	return rc;
}

void sysfs_deinit(struct sysfs_cxt *cxt)
{
	if (!cxt)
		return;

	if (cxt->dir_fd >= 0)
	       close(cxt->dir_fd);
	free(cxt->dir_path);

	memset(cxt, 0, sizeof(*cxt));

	cxt->dir_fd = -1;
}

static int sysfs_open(struct sysfs_cxt *cxt, const char *attr)
{
	int fd = open_at(cxt->dir_fd, cxt->dir_path, attr, O_RDONLY|O_CLOEXEC);

	if (fd == -1 && errno == ENOENT &&
	    strncmp(attr, "queue/", 6) == 0 && cxt->parent) {

		/* Exception for "queue/<attr>". These attributes are available
		 * for parental devices only
		 */
		fd = open_at(cxt->parent->dir_fd, cxt->dir_path, attr,
				O_RDONLY|O_CLOEXEC);
	}
	return fd;
}

static FILE *sysfs_fopen(struct sysfs_cxt *cxt, const char *attr)
{
	int fd = sysfs_open(cxt, attr);

	return fd < 0 ? NULL : fdopen(fd, "r" UL_CLOEXECSTR);
}

int sysfs_scanf(struct sysfs_cxt *cxt,  const char *attr, const char *fmt, ...)
{
	FILE *f = sysfs_fopen(cxt, attr);
	va_list ap;
	int rc;

	if (!f)
		return -EINVAL;
	va_start(ap, fmt);
	rc = vfscanf(f, fmt, ap);
	va_end(ap);

	fclose(f);
	return rc;
}

int sysfs_read_u64(struct sysfs_cxt *cxt, const char *attr, uint64_t *res)
{
	uint64_t x = 0;

	if (sysfs_scanf(cxt, attr, "%"SCNu64, &x) == 1) {
		if (res)
			*res = x;
		return 0;
	}
	return -1;
}
#ifdef TEST_PROGRAM_SYSFS
#include <errno.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	struct sysfs_cxt cxt = UL_SYSFSCXT_EMPTY;
	char *devname;
	dev_t devno;
	char path[PATH_MAX];
	int i, is_part;
	uint64_t u64;
	ssize_t len;

	if (argc != 2)
		errx(EXIT_FAILURE, "usage: %s <devname>", argv[0]);

	devname = argv[1];
	devno = sysfs_devname_to_devno(devname, NULL);

	if (!devno)
		err(EXIT_FAILURE, "failed to read devno");

	is_part = sysfs_devno_has_attribute(devno, "partition");

	printf("NAME: %s\n", devname);
	printf("DEVNO: %u (%d:%d)\n", (unsigned int) devno, major(devno), minor(devno));
	printf("DEVNOPATH: %s\n", sysfs_devno_path(devno, path, sizeof(path)));
	printf("DEVPATH: %s\n", sysfs_devno_to_devpath(devno, path, sizeof(path)));
	printf("PARTITION: %s\n", is_part ? "YES" : "NOT");

	if (sysfs_init(&cxt, devno, NULL))
		return EXIT_FAILURE;

	len = sysfs_readlink(&cxt, NULL, path, sizeof(path) - 1);
	if (len > 0) {
		path[len] = '\0';
		printf("DEVNOLINK: %s\n", path);
	}

	if (!is_part) {
		printf("First 5 partitions:\n");
		for (i = 1; i <= 5; i++) {
			dev_t dev = sysfs_partno_to_devno(&cxt, i);
			if (dev)
				printf("\t#%d %d:%d\n", i, major(dev), minor(dev));
		}
	}

	printf("SLAVES: %d\n", sysfs_count_dirents(&cxt, "slaves"));

	if (sysfs_read_u64(&cxt, "size", &u64))
		printf("read SIZE failed\n");
	else
		printf("SIZE: %jd\n", u64);

	if (sysfs_read_int(&cxt, "queue/hw_sector_size", &i))
		printf("read SECTOR failed\n");
	else
		printf("SECTOR: %d\n", i);

	printf("DEVNAME: %s\n", sysfs_get_devname(&cxt, path, sizeof(path)));

	sysfs_deinit(&cxt);
	return EXIT_SUCCESS;
}
#endif

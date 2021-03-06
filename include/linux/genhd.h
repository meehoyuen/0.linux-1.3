#ifndef _LINUX_GENHD_H
#define _LINUX_GENHD_H

/*
 * 	genhd.h Copyright (C) 1992 Drew Eckhardt
 *	Generic hard disk header file by  
 * 		Drew Eckhardt
 *
 *		<drew@colorado.edu>
 */

#define CONFIG_MSDOS_PARTITION 1

#ifdef __alpha__
#define CONFIG_OSF_PARTITION 1
#endif

#ifdef __sparc__
#define CONFIG_SUN_PARTITION 1
#endif

/* These two have identical behaviour; use the second one if DOS fdisk gets
   confused about extended/logical partitions starting past cylinder 1023. */	
#define DOS_EXTENDED_PARTITION 5
#define LINUX_EXTENDED_PARTITION 0x85

#define DM6_PARTITION		0x54	/* has DDO: use xlated geom & offset */
#define EZD_PARTITION		0x55	/* EZ-DRIVE:  same as DM6 (we think) */
#define DM6_AUX1PARTITION	0x51	/* no DDO:  use xlated geom */
#define DM6_AUX3PARTITION	0x53	/* no DDO:  use xlated geom */
	
struct partition {
	unsigned char boot_ind;		/* 0x80 - active */
	unsigned char head;		/* starting head */
	unsigned char sector;		/* starting sector */
	unsigned char cyl;		/* starting cylinder */
	unsigned char sys_ind;		/* What partition type */
	unsigned char end_head;		/* end head */
	unsigned char end_sector;	/* end sector */
	unsigned char end_cyl;		/* end cylinder */
	unsigned int start_sect;	/* starting sector counting from 0 */
	unsigned int nr_sects;		/* nr of sectors in partition */
};

struct hd_struct {
	long start_sect;
	long nr_sects;
};

struct gendisk {
	int major;			/* major number of driver */
	const char *major_name;		/* name of major driver */
	int minor_shift;		/* number of times minor is shifted to
					   get real minor */
	int max_p;			/* maximum partitions per device */
	int max_nr;			/* maximum number of real devices */

	void (*init)(struct gendisk *);	/* Initialization called before we do our thing */
	struct hd_struct *part;		/* partition table */
	int *sizes;			/* device size in blocks, copied to blk_size[] */
	int nr_real;			/* number of real devices */

	void *real_devices;		/* internal use */
	struct gendisk *next;
};

extern struct gendisk *gendisk_head;	/* linked list of disks */

/*
 * disk_name() is used by genhd.c and md.c.
 * It formats the devicename of the indicated disk
 * into the supplied buffer, and returns a pointer
 * to that same buffer (for convenience).
 */
char *disk_name (struct gendisk *hd, int minor, char *buf);

#endif

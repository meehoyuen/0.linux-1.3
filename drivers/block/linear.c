
/*
   linear.c : Multiple Devices driver for Linux
              Copyright (C) 1994-96 Marc ZYNGIER
	      <zyngier@ufr-info-p7.ibp.fr> or
	      <maz@gloups.fdn.fr>

   Linear mode management functions.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   You should have received a copy of the GNU General Public License
   (for example /usr/src/linux/COPYING); if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#include <linux/module.h>

#include <linux/md.h>
#include <linux/linear.h>
#include <linux/malloc.h>

#define MAJOR_NR MD_MAJOR
#define MD_DRIVER
#define MD_PERSONALITY

#include <linux/blk.h>

static int linear_run (int minor, struct md_dev *mddev)
{
  int cur=0, i, size, dev0_size, nb_zone;
  struct linear_data *data;

  MOD_INC_USE_COUNT;
  
  mddev->private=kmalloc (sizeof (struct linear_data), GFP_KERNEL);
  data=(struct linear_data *) mddev->private;

  /*
     Find out the smallest device. This was previously done
     at registry time, but since it violates modularity,
     I moved it here... Any comment ? ;-)
   */

  data->smallest=devices[minor];
  for (i=1; i<mddev->nb_dev; i++)
    if (data->smallest->size > devices[minor][i].size)
      data->smallest=devices[minor]+i;
  
  nb_zone=data->nr_zones=
    md_size[minor]/data->smallest->size +
    (md_size[minor]%data->smallest->size ? 1 : 0);
  
  data->hash_table=kmalloc (sizeof (struct linear_hash)*nb_zone, GFP_KERNEL);

  size=devices[minor][cur].size;

  i=0;
  while (cur<mddev->nb_dev)
  {
    data->hash_table[i].dev0=devices[minor]+cur;

    if (size>=data->smallest->size) /* If we completely fill the slot */
    {
      data->hash_table[i++].dev1=NULL;
      size-=data->smallest->size;

      if (!size)
      {
	if (++cur==mddev->nb_dev) continue;
	size=devices[minor][cur].size;
      }

      continue;
    }

    if (++cur==mddev->nb_dev) /* Last dev, set dev1 as NULL */
    {
      data->hash_table[i].dev1=NULL;
      continue;
    }

    dev0_size=size;		/* Here, we use a 2nd dev to fill the slot */
    size=devices[minor][cur].size;
    data->hash_table[i++].dev1=devices[minor]+cur;
    size-=(data->smallest->size - dev0_size);
  }

  return 0;
}

static int linear_stop (int minor, struct md_dev *mddev)
{
  struct linear_data *data=(struct linear_data *) mddev->private;
  
  kfree (data->hash_table);
  kfree (data);

  MOD_DEC_USE_COUNT;

  return 0;
}


static int linear_map (int minor, struct md_dev *mddev, struct request *req)
{
  struct linear_data *data=(struct linear_data *) mddev->private;
  struct linear_hash *hash;
  struct real_dev *tmp_dev;
  long block, rblock;
  struct buffer_head *bh, *bh2;
  int queue, nblk;
  static struct request pending[MAX_REAL]={{0, }, };

  while (req->nr_sectors)
  {
    block=req->sector >> 1;
    hash=data->hash_table+(block/data->smallest->size);
    
    if (block >= (hash->dev0->size + hash->dev0->offset))
    {
      if (!hash->dev1)
	printk ("linear_map : hash->dev1==NULL for block %ld\n", block);
      tmp_dev=hash->dev1;
    }
    else
      tmp_dev=hash->dev0;
    
    if (block >= (tmp_dev->size + tmp_dev->offset) || block < tmp_dev->offset)
      printk ("Block %ld out of bounds on dev %04x size %d offset %d\n", block, tmp_dev->dev, tmp_dev->size, tmp_dev->offset);
    
    rblock=(block-(tmp_dev->offset));
    
    if (req->sem)				/* This is a paging request */
    {
      req->rq_dev=tmp_dev->dev;
      req->sector=rblock << 1;
      add_request (blk_dev+MAJOR (tmp_dev->dev), req);
      
      return REDIRECTED_REQ;
    }

    queue=tmp_dev - devices[minor];

    for (nblk=0, bh=bh2=req->bh;
	 bh && rblock + nblk + (bh->b_size >> 10) <= tmp_dev->size;
	 nblk+=bh->b_size >> 10, bh2=bh, bh=bh->b_reqnext)
    {
      if (!buffer_locked(bh))
	printk("md%d: block %ld not locked\n", minor, bh->b_blocknr);
      
      bh->b_rdev=tmp_dev->dev;
    }

    pending[queue].rq_dev=tmp_dev->dev;
    pending[queue].cmd=req->cmd;
    pending[queue].sector=rblock << 1;
    pending[queue].nr_sectors=nblk << 1;
    pending[queue].current_nr_sectors=req->bh->b_size >> 9;
    pending[queue].bh=req->bh;
    pending[queue].bhtail=bh2;
    bh2->b_reqnext=NULL;
    
    req->bh=bh;
    req->sector+=nblk << 1;
    req->nr_sectors-=nblk << 1;
  }

  req->rq_status=RQ_INACTIVE;
  wake_up (&wait_for_request);
  make_md_request (pending, mddev->nb_dev);
  return REDIRECTED_REQ;
}


static int linear_status (char *page, int minor, struct md_dev *mddev)
{
  int sz=0;

#undef MD_DEBUG
#ifdef MD_DEBUG
  int j;
  struct linear_data *data=(struct linear_data *) mddev->private;
  
  sz+=sprintf (page+sz, "      ");
  for (j=0; j<data->nr_zones; j++)
  {
    sz+=sprintf (page+sz, "[%s",
		 partition_name (data->hash_table[j].dev0->dev));

    if (data->hash_table[j].dev1)
      sz+=sprintf (page+sz, "/%s] ",
		   partition_name(data->hash_table[j].dev1->dev));
    else
      sz+=sprintf (page+sz, "] ");
  }

  sz+=sprintf (page+sz, "\n");
#endif
  return sz;
}


static struct md_personality linear_personality=
{
  "linear",
  linear_map,
  linear_run,
  linear_stop,
  linear_status,
  NULL,				/* no ioctls */
  0
};


#ifndef MODULE

void linear_init (void)
{
  register_md_personality (LINEAR, &linear_personality);
}

#else

int init_module (void)
{
  return (register_md_personality (LINEAR, &linear_personality));
}

void cleanup_module (void)
{
  if (MOD_IN_USE)
    printk ("md linear : module still busy...\n");
  else
    unregister_md_personality (LINEAR);
}

#endif

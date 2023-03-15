/************* mount_umount.c file **************/

#ifndef __MOUNTUMOUNT_C__
#define __MOUNTUMOUNT_C__

#include "mkdir_creat.c"

int mount(char *filesys, char *mntp)
{

  MTABLE* mtptr;
  for (int i = 0; i < 8; i++){
    if(mountTable[i].dev == 0){
      mtptr = &mountTable[i];
      mtptr->dev = 0;
      break;
    }
  }
  int fd = open(filesys, O_RDWR);
  if(fd < 0 ){
    printf("filesys won't open\n");
    return -1;
  }
  char buf[BLKSIZE];
  get_block(globalDev, 1, buf);
  sp = (SUPER*)buf;

  // check magic number /
  if (sp->s_magic != SUPER_MAGIC){
    printf("super magic=%x : %s is not an EXT2 filesys\n",
    sp->s_magic, rootdev);
    exit(0);
  }
  int ino = getino(mntp);
  if (ino == 0){
    printf("Could not get ino!\n");
    return -1;
  }
  MINODE *mip = iget(globalDev, ino);
  if (!S_ISDIR(mip->INODE.i_mode)){
    printf("It is not a directory!\n");
    return -1;
  }
  if(mip->refCount > 2){
    printf("It is busy!\n");
    return -1;
  }
  mtptr->dev = fd;
  mtptr->ninodes = sp->s_inodes_count;
  mtptr->nblocks = sp->s_blocks_count;
  strcpy(mtptr->devName, filesys);
  strcpy(mtptr->mntName, "/");
  get_block(globalDev, 2, buf);
  gp = (GD *)buf;
  mtptr->bmap = gp->bg_inode_bitmap;
  mtptr->imap = gp->bg_inode_bitmap;

  mip->mounted = 1;
  mip->mptr = mtptr;
  mtptr->mntDirPtr = mip;
  
}

int umount(char *pathname)
{
  return 0;
}

#endif
/****************************************************************************
*                   KCW: mount root file system                             *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

#include "type.h"
#include "cd_ls_pwd.c"
#include "symlink.c"
#include "write_cp.c"
#include "mount_umount.c"
#include "misc1.c"
#include "mkdir_creat.c"
#include "link_unlink.c"


extern MINODE *iget();

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char gpath[128]; // global for tokenized components
char *name[64];  // assume at most 64 components in pathname
int   n;         // number of component strings

int  fd;
int  nblocks, ninodes, bmap, imap, iblock;
char line[128], cmd[32], obj[32], pathname[128];


int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i+1;           // pid = 1, 2
    p->uid = p->gid = 0;    // uid = 0: SUPER user
    p->cwd = 0;             // CWD of process
  }
}

/*
During system initialization to mount the root file system.
it reads the superblock of the root device to verify the device
is a valid EXT2 file system. then it loads the root INODE(ino = 2) of the
root device into a minode andsets the root pointer to the root minode.
It also sets the CWD of all PROCs to the root minode
A mount table entry is allocated to record the mounted root file system.
Some key infromation of the root device, such as the number of inodes and blocks,
the starting blocks of the bitmaps and inodes table, are also recorded in the
mount table for quick access.
*/

// load root INODE and set root pointer to it
int mount_root()
{  
  printf("mount_root()\n");
  root = iget(globalDev, 2);
}

char *disk = "disk2";     // change this to YOUR virtual

int main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){
    printf("open %s failed\n", disk);
    exit(1);
  }

  globalDev = fd;    // global dev same as this fd   

  /********** read super block  ****************/
  get_block(globalDev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(globalDev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblock = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, iblock);

  init();  
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->cwd = iget(globalDev, 2);
  printf("root refCount = %d\n", root->refCount);

  // WRTIE code here to create P1 as a USER process
  
  while(1){
    printf("input command : [ls|cd|pwd|quit|mkdir|creat|rmdir|link|symlink|unlink\nopen|cat|cp|pfd|close|write\n|mount] ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    if (line[0]==0)
       continue;
    pathname[0] = 0;

    sscanf(line, "%s %s %s", cmd, pathname, obj);
    //printf("cmd=%s pathname=%s\n", cmd, pathname, obj);
  
    if (strcmp(cmd, "ls")==0)
       my_ls(pathname);  
    else if (strcmp(cmd, "cd")==0)
       chdir(pathname);
    else if (strcmp(cmd, "pwd")==0)
       pwd(running->cwd);
    else if (strcmp(cmd, "quit")==0)
       quit();
    else if(strcmp(cmd, "mkdir") == 0)
       make_dir(pathname);
    else if(strcmp(cmd, "creat") == 0)
       creat_file(pathname);
    else if(strcmp(cmd, "rmdir") == 0)
      remove_dir(pathname);
    else if(strcmp(cmd, "link")==0)
      my_link(pathname, obj);
    else if(strcmp(cmd, "symlink") == 0)
      symlink_file(pathname, obj);
    else if(strcmp(cmd, "unlink") == 0)
      my_unlink(pathname);
    else if(strcmp(cmd, "open") == 0)
      open_file(pathname, obj);
    else if(strcmp(cmd, "pfd") == 0)
      pfd();
    else if(strcmp(cmd, "write") == 0)
      write_file(pathname, obj);
    else if(strcmp(cmd, "cat") == 0)
      cat_file(pathname);
    else if(strcmp(cmd, "close") == 0)
      close_file(pathname);
    else if(strcmp(cmd, "cp") == 0)
      my_cp(pathname);
    else if(strcmp(cmd, "mount") == 0)
      mount(pathname, obj);
    
  }
}

int quit()
{
  int i;

  for (i=0; i<NMINODE; i++)
  {
    MINODE *mip = &minode[i];
    if (mip->refCount && mip->dirty)
    {
      mip->refCount = 1;
      iput(mip);
    }
  }

  exit(0);
}

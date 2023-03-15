/*********** util.c file ****************/

/*
This file contains commonly used utility functions of the file system.
The most important utility functions are read/write disk block functions, iget(),iput() and getino(),
*/


#ifndef __UTIL_C__
#define __UTIL_C__

#include "type.h"

// extern MINODE minode[NMINODE];
// extern MINODE *root;
// extern PROC   proc[NPROC], *running;

// extern char gpath[128];
// extern char *name[64];
// extern int n;

// extern int fd, dev;
// extern int nblocks, ninodes, bmap, imap, iblk;

// extern char line[128], cmd[32], pathname[128];

/*
get_block/put_block functions:
a real or virtual disk, can read or write in unit of block size.
for real disks, this is due to hardware constraints.
for virtual disks, assume that read/write is also by block size, so that the code
can be ported to real disks if desired.
For a virtual disk, open it for R/W mode and use the file descriptor as the device
number.
fuctions: read/write a virtual disk block into/from a buufer area in memory.




*/

int get_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2
  lseek(dev, (long)blk*BLKSIZE, 0);
  read(dev, buf, BLKSIZE);
  return 1;
}

int put_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2
  lseek(dev, (long)blk*BLKSIZE, 0);
  write(dev, buf, BLKSIZE);
  return 1;
}

int tst_bit(char *buf, int bit) // pg. 333
{
  return buf[bit/8] & (1 << (bit % 8));
  //return 1; // Eventually: return proper test value
}

int set_bit(char *buf, int bit) // pg. 333
{
  // return 1;
  buf[bit/8] |= (1 << (bit % 8));

}

int clr_bit(char *buf, int bit) // pg. 306
{
  buf[bit/8] &= ~(1 << (bit%8));
}

MTABLE * get_mtable(int dev)
{
  return 0;
}

int incFreeInodes(int dev)
{
  char buf[BLKSIZE];
  // inc free inodes count in SUPER and GD

  get_block(dev, 1, buf);
  sp = (SUPER*)buf;
  sp->s_free_inodes_count++; //increment
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);
  gp = (GD*)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
  

  return 1;
}

// int idalloc(int dev, int ino)
// {
//   int i;
//   char buf[BLKSIZE];
//   MTABLE * mp = (MTABLE*)get_mtable(dev);
//   if(ino > ninodes)
//   {
//     printf("inumber %d out of range\n", ino);
//     return;
//   }

//   // get indoe bitmap block
//   get_block(dev, imap, buf);
//   clr_bit(buf, ino-1);
//   // write buf back
//   put_block(dev, imap, buf);
//   // update free inode count is SUPER and GD
//   incFreeInodes(dev);

//  return 1;
// }

int incFreeBlocks(int dev)
{
  return 1;
}

// int bdalloc(int dev, int bno)
// {
//   int i;
//   char buf[BLKSIZE];
//   MTABLE * mp = (MTABLE*)get_mtable(dev);
//   if(bno > ninodes)
//   {
//     printf("bnumber %d out of range\n", bno);
//     return;
//   }
//   // get bnode bitmap block
//   get_block(dev, imap, buf);
//   clr_bit(buf, bno-1);
//   // write buf back
//   put_block(dev, imap, buf);
//   // update free bnode count is SUPER and GD
//   incFreeInodes(dev);

//   return 1;
// }

int decFreeInodes(int dev)
{
  // Code in Chapter 11.8.1
  char buf[BLKSIZE];
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);

  return 1;
}

int decFreeBlocks(int dev)
{
  return 1;
}

// int ialloc(int dev)
// {
//   // Code in Chapter 11.8.1
//    int i;
//   char buf[BLKSIZE];
//   // use imap, ninodes in mount table of dev
//   //MTABLE *mp = (MTABLE *)get_mtable(dev);
//   printf("past mp\n");
//   get_block(dev, imap, buf);
//   printf("past block\n");
//   //printf("inde: %d", ninodes);
//   for (i=0; i< ninodes; i++){
//   if (tst_bit(buf, i)==0){
//   printf("past got into the for loop\n");
//   set_bit(buf, i);
//   put_block(dev, imap, buf);
//   // update free inode count in SUPER and GD
//   decFreeInodes(dev);
//   return (i+1);
//   }
// }
//   return 1;
// }

// int balloc(int dev)
// {
//   int i;
//   char buf[BLKSIZE];
//   // use imap, ninodes in mount table of devs
//   MTABLE *mp = (MTABLE *)get_mtable(dev);
//   get_block(dev, imap, buf);
//   for (i=0; i < nblocks; i++){
//   if (tst_bit(buf, i)==0){
//   set_bit(buf, i);
//   decFreeInodes(dev);
//   put_block(dev, imap, buf);
//   // update free inode count in SUPER and GD
  
//   return (i+1);
//   }
// }
//   return 1;
// }

MINODE *mialloc()
{
  // Code in Chapter 11.7.1

  return 0; // Eventually: Return entire MINODE *
}

int midalloc(MINODE *mip)
{
  // Code in Chapter 11.7.1

  return 1;
}

int tokenize(char *pathname)
{
  // copy pathname into gpath[]; tokenize it into name[0] to name[n-1]
  // Code in Chapter 11.7.2
  int i;
  char *s;
  printf("tokenize %s\n", pathname);

  strcpy(gline, pathname);   // tokens are in global gpath[ ]
  nname = 0;

  s = strtok(gline, "/");
  while(s){
    name[nname] = s;
    nname++;
    s = strtok(0, "/");
  }
  name[nname] = 0;
  
  for (i= 0; i<nname; i++)
    printf("%s  ", name[i]);
  printf("\n");
  return 1; // Eventually: Return number of names in name[]
}

/*
This function returns a pointer to the in-memory minode containing the
INODE of (dev, ino). The returned minode is unique.
Only one copy of the INODE exists in memory.




*/

MINODE *iget(int dev, int ino)
{
  // return minode pointer of loaded INODE=(dev, ino)
  // Code in Chapter 11.7.2

  int i;
  MINODE *mip;
  char buf[BLKSIZE];
  int blk, offset;
  INODE *ip;

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount && mip->dev == dev && mip->ino == ino){
       mip->refCount++;
       //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
       return mip;
    }
  }
    
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount == 0){
       //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
       mip->refCount = 1;
       mip->dev = dev;
       mip->ino = ino;

       // get INODE of ino into buf[ ]    
       blk    = (ino-1)/8 + iblock;
       offset = (ino-1) % 8;

       //printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

       get_block(dev, blk, buf);    // buf[ ] contains this INODE
       ip = (INODE *)buf + offset;  // this INODE in buf[ ] 
       // copy INODE to mp->INODE
       mip->INODE = *ip;
       return mip;
    }
  }   
  printf("PANIC: no more free minodes\n");
  return 0;
}

/*
This function releases a used minode pointed by mip.
each minode has a refCount, which represents the number of users that are
using the minode. iput() decrements the refCount by 1.
if the refCount is non-zero then the minode still has other users.
the caller simply return. if the caller is the last user of the minode (refcount = 0),
the INODE is written back to disk if it is modified (dirty)
*/

void iput(MINODE *mip)
{
  INODE *ip;
  int i, block, offset;
  char buf[BLKSIZE];
  if (mip==0) return;
  mip->refCount--; // dec refCount by 1
  if (mip->refCount > 0) return; // still has user
  if (mip->dirty == 0) return; // no need to write back
  // write INODE back to disk
  block = (mip->ino - 1) / 8 + iblock;
  offset = (mip->ino - 1) % 8;
  // get block containing this inode
  get_block(mip->dev, block, buf);
  ip = (INODE *)buf + offset; // ip points at INODE
  *ip = mip->INODE; // copy INODE to inode in block
  put_block(mip->dev, block, buf); // write back to disk
  midalloc(mip); // mip->refCount = 0;

}


int search(MINODE *mip, char *name)
{
  // search for name in (DIRECT) data blocks of mip->INODE
  // return its ino
  // Code in Chapter 11.7.2

  int i;
  char *cp, temp[256], sbuf[BLKSIZE];
  DIR *dp;
  for (i=0; i<12; i++){ // search DIR direct blocks only
  if (mip->INODE.i_block[i] == 0)
  return 0;
  get_block(mip->dev, mip->INODE.i_block[i], sbuf);
  dp = (DIR *)sbuf;
  cp = sbuf;
  while (cp < sbuf + BLKSIZE){
  strncpy(temp, dp->name, dp->name_len);
  temp[dp->name_len] = 0;
  printf("%8d%8d%8u %s\n",
  dp->inode, dp->rec_len, dp->name_len, temp);
  if (strcmp(name, temp)==0){
  printf("found %s : inumber = %d\n", name, dp->inode);
  return dp->inode;
  }
  cp += dp->rec_len;
  dp = (DIR *)cp;
  }
  }
  return 0;
}

int getino(char *pathname)
{
  // return ino of pathname
  // Code in Chapter 11.7.2
  MINODE *mip;
  int i, ino;
  if (strcmp(pathname, "/")==0){
  return 2; // return root ino=2
  }
  if (pathname[0] == '/')
    mip = root; // if absolute pathname: start from root
  else
      mip = running->cwd; // if relative pathname: start from CWD
  mip->refCount++; // in order to iput(mip) later
  tokenize(pathname); // assume: name[ ], nname are globals
  for (i=0; i<nname; i++){ // search for each component string
  if (!S_ISDIR(mip->INODE.i_mode)){ // check DIR type
  printf("%s is not a directory\n", name[i]);
  iput(mip);
  return 0;
  }
  ino = search(mip, name[i]);
  if (!ino){
  printf("no such component name %s\n", name[i]);
  iput(mip);
  return 0;
  }
  iput(mip); // release current minode
  mip = iget(globalDev, ino); // switch to new minode
  }
  iput(mip);
  return ino;
  
}


int findmyname(MINODE *parent, u32 myino, char* myname) 
{
  // WRITE YOUR code here:
  // search parent's data block for myino;
  // copy its name STRING to myname[ ];
  char buf[BLKSIZE], temp[256], *cp;
  DIR* dp;
  MINODE* mip = parent;


  for(int i = 0; i < 12; i++) //searching direct block only
  {
    if(mip->INODE.i_block[i] == 0)
      return 0;
    get_block(mip->dev, mip->INODE.i_block[i], buf);
    dp = (DIR*) buf;
    cp = buf;

    while(cp < buf + BLKSIZE)
    {
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      //printf("temp: %s\n", temp);
      if(dp->inode == myino)
      {
        strcpy(myname, temp);
        return 1;
      }

      cp += dp->rec_len;
      dp = (DIR*) cp;
      
    }
  }

  return 0;
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
  // mip->a DIR minode. Write YOUR code to get mino=ino of .
  //                                         return ino of ..

  char buf[BLKSIZE],*cp;
  DIR * dp;
  get_block(globalDev, mip->INODE.i_block[0], buf);
  cp = buf;
  dp = (DIR *)buf;
  *myino = dp->inode; //myino
  cp += dp->rec_len;
  dp = (DIR*)cp;
  //printf("dn: %s", dp->name);
  return dp->inode;

}

#endif
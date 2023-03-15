#include "type.h"

int ialloc(int dev)
{
  // Code in Chapter 11.8.1
   int i;
  char buf[BLKSIZE];
  // use imap, ninodes in mount table of dev
  //MTABLE *mp = (MTABLE *)get_mtable(dev);
  printf("past mp\n");
  get_block(dev, imap, buf);
  printf("past block\n");
  //printf("inde: %d", ninodes);
  for (i=0; i< ninodes; i++){
  if (tst_bit(buf, i)==0){
  printf("past got into the for loop\n");
  set_bit(buf, i);
  put_block(dev, imap, buf);
  // update free inode count in SUPER and GD
  decFreeInodes(dev);
  return (i+1);
  }
}
  return 1;
}

int balloc(int dev)
{
  int i;
  char buf[BLKSIZE];
  // use imap, ninodes in mount table of devs
  MTABLE *mp = (MTABLE *)get_mtable(dev);
  get_block(dev, imap, buf);
  for (i=0; i < nblocks; i++){
  if (tst_bit(buf, i)==0){
  set_bit(buf, i);
  decFreeInodes(dev);
  put_block(dev, imap, buf);
  // update free inode count in SUPER and GD
  
  return (i+1);
  }
}
  return 1;
}

int idalloc(int dev, int ino)
{
  int i;
  char buf[BLKSIZE];
  MTABLE * mp = (MTABLE*)get_mtable(dev);
  if(ino > ninodes)
  {
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get indoe bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, ino-1);
  // write buf back
  put_block(dev, imap, buf);
  // update free inode count is SUPER and GD
  incFreeInodes(dev);

 return 1;
}

int bdalloc(int dev, int bno)
{
  int i;
  char buf[BLKSIZE];
  MTABLE * mp = (MTABLE*)get_mtable(dev);
  if(bno > ninodes)
  {
    printf("bnumber %d out of range\n", bno);
    return;
  }
  // get bnode bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, bno-1);
  // write buf back
  put_block(dev, imap, buf);
  // update free bnode count is SUPER and GD
  incFreeInodes(dev);

  return 1;
}
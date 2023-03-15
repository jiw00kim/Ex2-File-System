/************* open_close_lseek.c file **************/

#ifndef __OPENCLOSELSEEK_C__
#define __OPENCLOSELSEEK_C__

#include "mkdir_creat.c"

int truncate(MINODE *mip)
{



  /*

  1. release mip->INODE's data blocks;
  a file may have 12 direct blocks, 256 indriect blocks and 256 * 256
  double indirect data blocks. release them all.
  2. Update INODE's time field
  3. Set INODE's size to 0 and mark Minode[] dirty.

  // deallocates the data blocks ofo a file and trancates file size to 0


  */

  INODE *ip = &(mip->INODE);
  char buf[BLKSIZE], buf2[BLKSIZE];
  int bufs[1024], buf2s[1024];
  // Direct Blocks
  printf("Deallocating direct blocks.\n");
  for(int i = 0; i < 12; i++)
  {
    if(ip->i_block[i])
    {
      bdalloc(globalDev, ip->i_block[i]); //deallocating each data block
    }
  }

  printf("\n");

  if(ip->i_block[12])
  {
    printf("Deallocating Indirect blocks\n");
    int blk = get_block(globalDev, ip->i_block[12], bufs);
    for(int i = 0; i < 256; i++)
    {
      if(bufs[i])
      {
        bdalloc(globalDev, bufs[i]);
      }
    }
  }

  printf("\n");

  if(ip->i_block[13])
  {
    printf("Deallocating double Indirect blocks\n");
    get_block(globalDev, ip->i_block[13], buf2s);
    for(int i = 0; i < 256256; i++)
    {
      bdalloc(globalDev, buf2s[i]);
    }
  }


  ip->i_size = 0;
  mip->dirty = 1;
  return 1;

}

int open_file(char *pathname, char *modes)
{
  // the mode has to be 0,1,2,3 R,W,RW,Append
  int mode = atoi(modes);
  printf("In open!\n");
  int ino = getino(pathname);
  if (ino == 0){
    printf("File does not exist!\n");
    return -1;
  }
  MINODE *mip = iget(globalDev, ino);

  printf("The mip: %d\n\n", mip->INODE.i_mode);
  if (!(S_ISREG(mip->INODE.i_mode))){
    printf("This is not a regular file!\n");
    return -1;
  }
  int cnt = 0;
  printf("In 1 !\n");

  while(cnt != NFD){
    printf("fdass\n");
    if(running->fd[cnt] ==NULL){
      break;
    }
    if (running->fd[cnt]->minodeptr == mip){
      printf("dfas\n");
      if(running->fd[cnt]->mode != 0){
        //this means its not read so it cant happen
        printf("File already opened with incompatible mode!\n");
        return -1;
      }
    }
    cnt++;
  }

  OFT *oftp = (OFT *)malloc(sizeof(NOFT));
  oftp->mode = mode;
  oftp->refCount = 1;
  oftp->minodeptr = mip;
  printf("mode: %d\n", mode);
  switch(mode){
    case 0:
      oftp->offset = 0;
      break;
    case 1:
      truncate(mip);
      oftp->offset = 0;
      break;
    case 2:
      oftp->offset = 0;
      break;
    case 3:
      oftp->offset = mip->INODE.i_size;
      break;
    default:
      printf("Invalid mode!\n");
      return -1;
  }

  cnt = 0;
  while (NFD != cnt){
    if(running->fd[cnt] == NULL){
      running->fd[cnt] = oftp;
      break;//once we break cnt will be the file descriptor
    }

    cnt++;
  }

  if(mode == 0){
    mip->INODE.i_atime = time(0L);
    mip->dirty = 1;
  }
  else{
    mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
    mip->dirty = 1;
  }
  iput(mip);
  return cnt;

}




int my_close(int fd)
{
  return 1;
}

// closes the file descriptor
int close_file(char * fds)
{
  printf("In Close\n");
  printf("fd in close: %s\n", fds);
  int fd = atoi(fds);
  printf("after fd\n");
  // check fd is a valid opened file descriptor;
  if(fd <0 || fd > NFD && running->fd[fd] == NULL)
  {
    printf("Invalid file descriptor.\n");
    return -1;
  }

  // if (PROC's fd[fd] != 0)
    OFT* oftp;
    oftp = running->fd[fd];
    running->fd[fd] = 0;
    oftp->refCount--;

    if(oftp->refCount > 0)
      return 0;

    iput(oftp->minodeptr);

}

int my_lseek(int fd, int position)
{
  // From fd, find the OFT entry

  if(running->fd[fd] == 0)
  {
    printf("fd is not opend\n");
    return -1;
  }

  if(position < 0 || position > running->fd[fd]->minodeptr->INODE.i_size)
  {
    printf("Invalid offset size");
    return -1;
  }
  int originalPosition = running->fd[fd]->offset;

  return originalPosition;
  return 1; // Eventually: return original position in file
}

int pfd()
{
   int cnt = 0;
   
    printf("FD   MODE   OFFSET   INODE\n");
    while (cnt != NFD){

      if(running->fd[cnt] != NULL){
        printf("%d    %d      %d        [%d, %d]\n", cnt, running->fd[cnt]->mode, running->fd[cnt]->offset, running->fd[cnt]->minodeptr->dev, running->fd[cnt]->minodeptr->ino);

      }

      cnt++;
    }
    return 1;

}

int dup(int fd)
{
    // verify fd is an opened descriptor;

    if(running->fd[fd] == 0)
    {
      printf("%d fd can't open\n", fd);
      return -1;
    }
    // duplicates copy fd[fd] into FIRST empty fd[] slot;
    for(int i = 0; i <NFD; i++)
    {
      if(running->fd[i] == 0)
      {
        running->fd[i] = running->fd[fd];
        break;
      }
    }

    // increment OFT's refCount by 1
    running->fd[fd]->refCount++;

    return 1;

}

int dup2(int fd, int gd)
{
    return 1;
}

#endif
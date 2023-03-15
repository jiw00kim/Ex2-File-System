/************* read_cat.c file **************/

#ifndef __READCAT_C__
#define __READCAT_C__

#include "open_close.c"

int read_file(int fd, int nbytes)
{
  if(fd != 0 || fd != 2){
    printf("The file descriptor is not for read or read/write!\n");
    return -1;
  }

  OFT* oftp = running->fd[fd];
  char* buf[BLKSIZE];


  if(oftp->mode != O_RDONLY){
    printf("It is not read only!\n");
    return -1;
  }
  if(oftp->mode != O_RDWR){
    printf("It is not read/write only!\n");
    return -1;
  }

  printf("reading %d bytes from fd = %d\n", nbytes, fd);
  return (my_read(fd, buf, nbytes));
  //return 1; // Eventually: Return the results of my_read
}

int my_read(int fd, char buf[], int nbytes)
{
  OFT *oftp = running->fd[fd];
  MINODE* mip = oftp->minodeptr;
  char* readbuf[BLKSIZE];
  int count = 0;
  int avil = mip->INODE.i_size - oftp->offset; //file size - OFT's offset
  char* cq = buf;
  int lbk = 0;
  int startByte = 0;
  int blk = 0;
  int remain = 0;
  int iblockNum = 0;
  int *bufint[256];
  int *bufint2[256];
  //printf("In read\n");
  while (nbytes && avil){
    //printf("gets in loop!\n");
    lbk = oftp->offset / BLKSIZE;
    startByte = oftp->offset % BLKSIZE;
    //printf("did mailmans!\n");
    if(lbk < 12){
      blk = mip->INODE.i_block[lbk];
     // printf("int if\n");
    }
    else if(lbk >= 12 && lbk <256+12){
      //indirect 13th block
      //a block of block numbers
      //printf("in elseif\n");
      get_block(globalDev, mip->INODE.i_block[12], (char*)bufint);
      //buf is a list of ints 
      //printf("past first block\n");
      iblockNum = bufint[lbk-12];//gets you to the correct position
      blk = iblockNum;
      // = mip->INODE.i_block[iblockNum];




    }
    else{
      //double indirect
      get_block(globalDev, mip->INODE.i_block[13], (char*)bufint);
      //buf is a list of ints 
      //printf("past second block!\n");
      iblockNum = bufint[(lbk-12 -256) / 256];//gets you to the correct position
      //blk = iblockNum;
      get_block(globalDev, iblockNum, (char*)bufint2);
      int diblockNum =bufint2[(lbk-12 -256) % 256];
      //diblockNum = bufint[lbk-12];//gets you to the correct position
      blk = diblockNum;
    }
    //printf("before third block\n");
    get_block(mip->dev, blk, readbuf);
    //printf("after third block\n");
    char *cp = readbuf + startByte;
    remain = BLKSIZE -startByte;



    //need a count for remaining
    //
    int temp;
    //printf("before while loop\n");
    while (remain > 0){
      //printf("in while\n");
            //strcpy(cq, cp);
          //  printf("redbuf: %s\n", readbuf);
            //printf("cq: %s\n", cq);
            *cq++ = *cp++;             // copy byte from readbuf[] into buf[]
            //printf("after cp\n");
             oftp->offset++;           // advance offset 
             count++;                  // inc count as number of bytes read
             avil--; nbytes--;  remain--;
             if (nbytes <= 0 || avil <= 0) 
                 break;
       }

       // if one data block is not enough, loop back to OUTER while for more...
  }


  return count;   // count is the actual number of bytes read


    //return 1; // Eventually: Return the actual number of bytes read
}

int cat_file(char *pathname)
{
 char mybuf[1024], dummy = 0;  // a null char at end of mybuf[ ]
  int n;
  // printf("Int cat!\n");
  int fd = open_file(pathname, "0");
  // printf("past fd\n");
  while( n = my_read(fd, mybuf, 1024)){
      //printf("n: %d\n\n", n);
       mybuf[n] = 0;             // as a null terminated string

       printf("%s", mybuf);   //<=== THIS works but not good
    //   spit out chars from mybuf[ ] but handle \n properly;

   } 

   printf("\n\nclose : refcount = %d\n", running->fd[fd]->refCount);
   printf("Read End of the file successfully\n");
   printf("fd: %d\n", fd);
  //  char* fds = (char)fd;
  char *fds=  (char*)malloc(20);
  sprintf(fds, "%d", fd);
  // sprintf(fds, "%s", fd);
  printf("outgoing fds: %s", fds);
   close_file(fds);

  return 1;
}

#endif
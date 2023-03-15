/************* symlink.c file **************/

#ifndef __SYMLINK_C__
#define __SYMLINK_C__

#include "mkdir_creat.c"

int symlink_file(char *old_file, char *new_file)
{
  int oino = getino(old_file);
  MINODE *oip = iget(globalDev, oino);

  if (oino == 0){
    printf("The old file does not exist!\n");
    return -1;
  }

  int newino = getino(new_file);
 

  if (newino != 0){
    printf("The new file already exists!");
    return -1;
  }
  creat_file(new_file);
  int nino = getino(new_file);
  MINODE* nip = iget(globalDev, nino);

  if (nino == 0){
    printf("The new file failed to create!");
    return -1;
  }
  nip->INODE.i_mode = 0xA1FF;
  int o_size = oip->INODE.i_size;




  strcpy(nip->INODE.i_block, old_file);

  nip->INODE.i_size = o_size;
  nip->dirty = 1;
  iput(nip);
  oip->dirty = 1;
  iput(nip);




  return 1;
}

int my_readlink(char *pathname)
{
  char buf[BLKSIZE];

  int ino = getino(pathname);

  if(ino == 0)
  {
    printf("File does not exist\n", pathname);
    return -1;
  }

  MINODE * nmip;
  nmip = iget(globalDev, ino);
  int j = 0;
  while(nmip->INODE.i_block[j])
  {
    for(int i = 0; i < 4; i++)
    {
      buf[i] = ((char*)&nmip->INODE.i_block[j]);
    }
    j++;
  }
  //return file size;
    return (strlen(buf));
}

#endif
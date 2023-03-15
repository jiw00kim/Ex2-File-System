/************* link_unlink.c file **************/

#ifndef __LINKUNLINK_C__
#define __LINKUNLINK_C__

#include "mkdir_creat.c"
#include "rmdir.c"

int my_link(char *fileName, char *linkName)
{

    MINODE *filePip;
    MINODE *linkPip;

    char *temp[BLKSIZE], *parent[BLKSIZE], *child[BLKSIZE];
    printf("Old file name: %s\n", fileName); // old file name = fileName
    printf("New file name:%s\n", linkName);

    // verify old_file exists and is not a DIR;
    int oino = getino(fileName);
    filePip = iget(globalDev, oino);

    // check omip->INODE file type (must not be DIR).
    if(oino == 0)
    {
        printf("Old file %s does not exist\n", fileName);
        return 0;
    }

    if(S_ISDIR(filePip->INODE.i_mode))
    {
        printf("%s Link not exist\n", fileName);
        iput(filePip);
        return 0;
    }
    if(getino(linkName) != 0)
    {
        printf("%s is already exist.\n", linkName);
        iput(filePip);
        return 0;
    }

    // create new_file with the same inosde number of old_file

    strcpy(temp, linkName);
    strcpy(parent, dirname(temp));
    stpcpy(temp, linkName);
    strcpy(child, basename(temp));
    printf("New file dir name = %s\n", parent);
    printf("New file base name: %s\n", child);

    int pino = getino(parent);
    linkPip = iget(globalDev, pino);
    // creat entry in new parent DIR with same inode number of old_file
    enter_name(linkPip, oino, child);

    //4 
    filePip->INODE.i_links_count++; //inc INODE's links_count by 1
    filePip->dirty = 1; //for write back by iput(omip)
    iput(filePip);
    iput(linkPip);


}

int link_file(char *pathname)
{
    return 1;
}

int my_unlink(char *pathname)
{
    int ino = getino(pathname);
    MINODE *mip = iget(globalDev, ino);

    //check this is a dir

    if(S_ISDIR(mip->INODE.i_mode)){
        printf("This is a directory, cannot unlink!\n");
        return -1;
    }

    char *dir[BLKSIZE];
    char *base[BLKSIZE];
    char *dname[BLKSIZE];
    char *bname[BLKSIZE];

    strcpy(dir, pathname);
    strcpy(base, pathname);

    // dividing pathname to dirname and basename
    strcpy(dname, dirname(dir));
    strcpy(bname, basename(base));

    printf("parent = %s  ", dname);
    printf("child = %s\n", bname);

    int pino = getino(dname);
    printf(" prints: %s %s \n", dname, bname);
    if(pino == 0)
    {
        printf("dirname = %s does not exist.\n", dname);
        return -1;
    }

    MINODE *pmip = iget(globalDev, pino);

    rm_child(pmip,bname);
    pmip->dirty = 1;
    iput(pmip);
    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count >0){
        mip->dirty =1;
    }
    else{
        bdalloc(mip->dev, mip->INODE.i_block[0]);
        idalloc(mip->dev, mip->ino);
    }
    iput(mip);

    return 1;
}

int my_rm(MINODE *mip, char *pathname)
{
    return 1;
}

#endif
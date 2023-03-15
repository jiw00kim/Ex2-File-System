/************* mkdir_creat.c file **************/

#ifndef __MKDIRCREAT_C__
#define __MKDIRCREAT_C__

#include "type.h"
#include "util.c"
//#include "alloc_dalloc.c"

int enter_name(MINODE *pip, int ino, char *name)
{
   
    char buf[BLKSIZE],*cp;
    int ideal_length,need_length = 0;
    int name_len = strlen(name);
    int remain = 0;

    
    
    for(int i = 0; i < 12; i++) // assume: only 12 direct blocks
    {
        ideal_length =4*((8 + name_len + 3)/4 ); //each dir_entry has an ideal length
    if (pip->INODE.i_block[i] == 0)
    {
        break;
    }
      
        get_block(pip->dev, pip->INODE.i_block[i], buf);
        dp = (DIR*)buf;
        cp = buf;
        // step to the last entry in the data block:
        while (cp + dp->rec_len < buf + BLKSIZE)
        {
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }

        ideal_length = 4*((8 + dp->name_len + 3)/4 );
        
        // dp NOW points at last entry in block
        remain = dp->rec_len - ideal_length;

        if (remain >= need_length)
        {
            dp->rec_len = ideal_length; // enter the new entry as the LAST entry 
            cp += dp->rec_len;
            dp = (DIR *)cp;

            strcpy(dp->name, name);
            dp->inode = ino;
            dp->name_len = name_len;
            dp->rec_len = remain;
            put_block(globalDev, pip->INODE.i_block[i], buf); // put into the block

        // enter the new entry as the LAST entry and
        // trim the previous entry rec_len to its ideal_length;
        // need help with this

        }

    }

    return 0;
}

int my_mkdir(MINODE *pip, char *name)
{

    int ino = ialloc(globalDev);
    int bno = balloc(globalDev);
    char buf[BLKSIZE];
   // MINODE mip = iget(globalDev, ino);

    MINODE *mip = iget(globalDev, ino);
    INODE *ip = &mip->INODE;
    pip->INODE.i_links_count++;
    ip->i_mode = 0x41ED; // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = bno; // new DIR has one data block
    //ip->i_block[1] to ip->i_block[14] = 0;
    for (int i = 1; i < 15; i++){
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; // mark minode dirty
    pip->dirty = 1;
    iput(mip);
    iput(pip); // write INODE to disk

    bzero(buf, BLKSIZE); // optional: clear buf[ ] to 0
    get_block(globalDev, bno,buf);

    //this makes . and .. entries
   // char buf[BLKSIZE];

    DIR *dp = (DIR*)buf;
    // make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';
    // make .. entry: pino=parent DIR ino, blk=allocated block
    dp = (char*)dp + 12;
    dp->inode = pip->ino;
    dp->rec_len = BLKSIZE-12; // rec_len spans block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(globalDev, bno, buf); // write to blk on diks
    enter_name(pip,ino,name);

    return 1;
}

int make_dir(char * pathname)
{
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
    
    if(pino == 0)
    {
        printf("dirname = %s does not exist.\n", dname);
        return -1;
    }

    MINODE * pmip = iget(globalDev, pino);


    // check pmip->INODE is a DIR

    if(!S_ISDIR(pmip->INODE.i_mode)) 
    {
        // if dirname is not a directory
        printf("What is pmip = %s\n", pmip);
        iput(pmip);
        return -1;
    }

    // check child not exist in parent directory

    if(search(pmip, bname) != 0)
    {
        printf("%s is already exist in %s\n", bname, dname);
        iput(pmip);
        return -1;
    }
    // call my_mkdir(pmip, basename) to create a DIR:
    my_mkdir(pmip, bname);
}

int my_creat(MINODE *pip, char *name)
{
    int ino = ialloc(globalDev);
    int bno = balloc(globalDev);
    char buf[BLKSIZE];
   // MINODE mip = iget(globalDev, ino);

    MINODE *mip = iget(globalDev, ino);
    INODE *ip = &mip->INODE;
    pip->INODE.i_links_count++;
    ip->i_mode = 0644; // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = 0; // size in bytes
    ip->i_links_count = 1; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = bno; // new DIR has one data block
    //ip->i_block[1] to ip->i_block[14] = 0;
    for (int i = 1; i < 15; i++){
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; // mark minode dirty
    pip->dirty = 1;
    iput(mip); // write INODE to disk
    iput(pip);
    enter_name(pip, ino, name);


    // bzero(buf, BLKSIZE); // optional: clear buf[ ] to 0
    // get_block(globalDev, bno,buf);

    //this makes . and .. entries
   // char buf[BLKSIZE];

    // DIR *dp = (DIR*)buf;
    // // make . entry
    // dp->inode = ino;
    // dp->rec_len = 12;
    // dp->name_len = 1;
    // dp->name[0] = '.';
    // // make .. entry: pino=parent DIR ino, blk=allocated block
    // dp = (char*)dp + 12;
    // dp->inode = pip->ino;
    // dp->rec_len = BLKSIZE-12; // rec_len spans block
    // dp->name_len = 2;
    // dp->name[0] = dp->name[1] = '.';
    // put_block(globalDev, bno, buf); // write to blk on diks
    // enter_name(pip,ino,name);

    return 1;
}

int creat_file(char *pathname)
{
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
    
    if(pino == 0)
    {
        printf("dirname = %s does not exist.\n", dname);
        return -1;
    }

    MINODE * pmip = iget(globalDev, pino);


    // check pmip->INODE is a DIR

    if(!S_ISDIR(pmip->INODE.i_mode)) 
    {
        // if dirname is not a directory
        printf("What is pmip = %s\n", pmip);
        iput(pmip);
        return -1;
    }

    // check child not exist in parent directory

    if(search(pmip, bname) != 0)
    {
        printf("%s is already exist in %s\n", bname, dname);
        iput(pmip);
        return -1;
    }
    // call my_mkdir(pmip, basename) to create a DIR:
    my_creat(pmip, bname);
}

#endif
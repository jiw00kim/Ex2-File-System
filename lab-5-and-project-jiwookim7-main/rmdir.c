/************* rmdir.c file **************/

#ifndef __RMDIR_C__
#define __RMDIR_C__

#include "open_close.c"


// remove child name from parent directory data block.
int rm_child(MINODE *pip, char *name) //name is child name
{
    // search parent INODE's data block(s) for the entry of name
    char *cp, buf[BLKSIZE], temp[BLKSIZE], pname[BLKSIZE];
    INODE * ip = &(pip->INODE);
    get_block(globalDev, ip->i_block[0], buf); // getting in parent data block into buf
    DIR * dp = (DIR*)buf;
    int size;

    cp = buf;
    DIR * prev = dp;

    while(cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len); // making name a string
        temp[dp->name_len] = 0;

        if(strcmp(name, temp)== 0)
        {
            printf("Found %s in parent's directory\n", name);
            break;
        }
        prev = dp;
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }

    // Delete name entry from parent dirctory 
    if(dp->rec_len == BLKSIZE) // if it is the first or only entry in parent data block
    {
        //deallocate data block
        bdalloc(globalDev, dp->inode);
        // reduce parent size by 1024
        pip->INODE.i_size -= BLKSIZE;
    }
    // if last entry in block
    else if((cp + dp->rec_len) >= (buf + BLKSIZE)) // if the pointer + my length = buff size
    {
        printf("Delete the last entry\n");
        // delete me and add your length to your predecesor
        int mylen = dp->rec_len;

        prev->rec_len += mylen; 
        // adding my length to my predecesor

        put_block(globalDev, ip->i_block[0], buf);
        pip->dirty = 1;
        iput(pip);

    }

    // entry is in the middle of a block
    else
    {
        printf("Delete the middle entry\n");

        int mylen = dp->rec_len;

        // iterate the parent data block to get the last entry dp2 will point to the last entry

        char *cp2 = cp;
        DIR * dp2 = (DIR*)cp2;

        while(cp2 + dp2->rec_len < buf + BLKSIZE)
        {
            cp2 += dp2->rec_len;
            dp2 = (DIR*)cp2;
        }
        // now dp2 points to the last entry
        // add mylen to the last
        dp2->rec_len += mylen; 
        // move to the left in parent data block
        size = (buf+BLKSIZE) - (cp + dp->rec_len);
        // move the whole chunk to the place where dp points at
        memmove(cp, dp->rec_len + cp, size);

        put_block(globalDev, ip->i_block[0], buf);

        pip->dirty = 1;
        iput(pip);

    }

    return 0;
}

int remove_dir(char *pathname)
{
    int ino = getino(pathname);
    MINODE *mip = iget(globalDev, ino);
    
    printf("past mip!\n");

    if(ino == 0)
    {
        printf("dirname does not exist!\n");
        return -1;
    }
//     verify INODE is a DIR (by INODE.i_mode field);
// minode is not BUSY (refCount = 1);
// verify DIR is empty (traverse data blocks for number of entries = 2);
    if (!S_ISDIR(mip->INODE.i_mode)){
        printf("It is not a directory!\n");
        return -1;
    }

    if (mip->refCount != 1){
        printf("Refcount is not 1!\n");
    }
    char* sbuf[BLKSIZE], *cp, temp_name[BLKSIZE];
    int temp_n = 0;
    INODE* temp = &mip->INODE;
    if (temp->i_links_count > 2){
        printf("There are more directories then intended, directory not empty!\n");
        return -1;
    }


    //pino = findino(); //get pino from .. entry in INODE.i_block[0]
    
    
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

    MINODE * pmip = iget(globalDev, pino);

    
    // check pmip->INODE is a DIR

    if(!S_ISDIR(pmip->INODE.i_mode)) 
    {
        // if dirname is not a directory
        iput(pmip);
        return -1;
    }
    char* name_file[256];
    //findmyname(pmip, ino, name_file);

    rm_child(pmip, bname); //need to complete rm_child
    pmip->INODE.i_links_count--;
    pmip->dirty = 1;
    iput(pmip);
    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);


    return 1;
}

#endif
// /************** lab5base.c file ******************/
// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <ext2fs/ext2_fs.h>
// #include <string.h>

// typedef struct ext2_super_block SUPER;
// typedef struct ext2_group_desc  GD;
// typedef struct ext2_inode       INODE;
// typedef struct ext2_dir_entry_2 DIR;

// SUPER *sp;
// GD    *gp;
// INODE *ip;
// DIR   *dp;   

// #define BLK 1024

// int fd;             // opened vdisk for READ
// int inodes_block;   // inodes start block number

// char gpath[128];    // token strings
// char *name[32];
// int n;

// int get_block(int fd, int blk, char *buf)
// {
//    lseek(fd, blk*BLK, 0);
//    read(fd, buf, BLK);
// }   

// int search(INODE *ip, char *name)
// {
//   // Chapter 11.4.4  int i; 
//   // Exercise 6
// }

// *************************************************************************/
// int tokenize(char *pathname)
// {
//   // you did this many times
// } 

// char *disk = "vdisk";

// int main(int argc, char *argv[])   // a.out pathname
// {
//   char buf[BLK];  // use more bufs if you need them

//   1. open vdisk for READ: print fd value
//   2. read SUPER block #1 to verify EXT2 fs : print s_magic in HEX
//   3. read GD block #2 to get inodes_block=bg_inode_table: print inodes_block 

//   4. read inodes_block into buf[] to get root INODE #2 : set ino=2 
//           INODE *ip = (INODE *)buf + 1;
			    
//   5. tokenize pathame=argv[1]);
     
//   6. for (i=0; i<n; i++){
//         printf("===========================================\n");
//         printf("search name[%d]=%s in ino=%d\n", i, name[i], ino);
//         ino = search(ip, name[i]);

//         if (ino==0){
//            printf("name %s does not exist\n", name[i]);
//            exit(1);
//         }
//         // MAILman's algrithm:
//         blk    = (ino-1) / 8 + inodes_block;
//         offset = (ino-1) % 8;
  
//         get_block(fd, blk, buf);

//         ip = (INODE *)buf + offset;
//    } 


// 7. HERE, ip->INODE of pathname
//    printf("****************  DISK BLOCKS  *******************\n");
//    for (i=0; i<15; i++){
//       if (ip->i_block[i]) 
//          printf("block[%2d] = %d\n", i, ip->i_block[i]);
//    }

//    printf("================ Direct Blocks ===================\n");
   
//    if (ip->i_block[12]){
//       printf("===============  Indirect blocks   ===============\n"); 
//    }

//    if (ip->i_block[13]){
//      printf("===========  Double Indirect blocks   ============\n"); 
//    }
// }

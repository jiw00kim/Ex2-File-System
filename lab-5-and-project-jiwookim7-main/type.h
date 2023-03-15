/*************** type.h file ************************/

/*
This file contains the data structure types of the EXT2 file system,
such as superblock, group, descriptor, indoe and dir entry structure.

*/

#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
#define BLKSIZE  1024

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

// Define block numbers of important EXT2 file system blocks on the disk
#define SUPERBLOCK  1
#define GDBLOCK     2
#define ROOT_INODE  2

// Default directories and regular file modes
#define DIR_MODE    0x41ED
#define FILE_MODE   0xB1AE
#define SUPER_MAGIC 0xEF53
#define SUPER_USER  0

// Define process statuses
#define FREE        0 // A free process has status 0
#define BUSY        1 // A busy process has status 1
#define READY       1 // ? (it started as 1)

// File system table sizes
#define NMINODE   100
#define NMTABLE    10
#define NPROC       2
#define NFD        16
#define NOFT       40

// inode
typedef struct minode{
  INODE INODE; // disk inode
  int dev, ino;
  int refCount; // use count
  int dirty; // modified flag (minode has been modified)
  int mounted; // mounted flag
  struct mtable *mptr; // mount table pointer
  // int lock; // ignored for simple file system
}MINODE;

// A structure for opening files
typedef struct oft{
  int  mode; // Mode of the file being opened
  int  refCount; // Number of procs using this instance of a file being opened
  MINODE *minodeptr; // Pointer to the minode of the file
  int  offset; // Byte offset for R|W (Read|Write)
}OFT;

// Process structure
typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid, gid;
  int          ppid;
  int          status;
  struct minode *cwd;
  OFT         *fd[NFD];
}PROC;

// Mount table structure
typedef struct mtable{
  int dev; // Device number, it will be 0 for FREE
  int ninodes; // From superblock
  int nblocks;
  int free_blocks; // From superblock and group descriptor GD
  int free_inodes;
  int bmap; // From group descriptor GD
  int imap;
  int iblock; // The start block of the inodes
  MINODE *mntDirPtr; // Mount point DIR pointer
  char devName[64]; // Name of device
  char mntName[64]; // Mount point DIR name
}MTABLE;

typedef struct Mount{
  int    dev;       // dev (opened vdisk fd number) 0 means FREE 
  int    ninodes;   // from superblock
  int    nblocks;
  int    bmap;      // from GD block
  int    imap;
  int    iblk;
  struct Minode *mounted_inode;
  char   name[64];  // device name, e.g. mydisk
  char   mount_name[64]; // mounted DIR pathname
} MOUNT;

MOUNT mountTable[8];  // set all dev = 0 in init()

MINODE minode[NMINODE]; // In memory inodes
MINODE *root; // Root minode
MTABLE mtable[NMTABLE]; // Mount tables

OFT oft[NOFT]; // Opened file instances
OFT *oftp;
PROC proc[NPROC]; // Process structures
PROC *running; //Currently running (executing) process

int ninodes, nblocks, bmap, imap, iblock;
int globalDev;

char *name[64]; // token string pointers
char gline[256]; // holds token strings, each pointed to by a name[i]

int nname; // number of token strings
int inode_start; // Inode start block

char *rootdev = "mydisk"; // Default root_device

#endif

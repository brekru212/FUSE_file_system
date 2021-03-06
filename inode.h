#ifndef INODE_H
#define INODE_H

#include <stdio.h>
//#include <ntsid.h>
#include <stddef.h>
#include <sys/stat.h> //need for mode_t to work on linux
#include<sys/types.h> //need for mode_t to work on linux
typedef struct inode {
    mode_t mode; // permission for owner

    uid_t user_id; // can be uint16_t, who created this file
    size_t size_of; // size of file/dir in bytes
    int is_file; // flag, if obj is file = 1, dir = 0
    // need for part2
    // st_atime
    // st_ctime
    // st_mtime
    // st_link
} inode;

//extern inode* inodes[256];
void inode_init(inode* cur_inode, mode_t mode, int is_file, size_t size);
void inode_free(inode* inode_ptr);
//int inode_insert(inode* cur_inode, inode* inodes[], int inode_bitmap[]);
int inode_bitmap_find_next_empty(int* inode_bitmap_ptr);
inode* inodes_addr();
int* inode_bitmap_addr();
void* single_inode_addr(int idx);
#endif

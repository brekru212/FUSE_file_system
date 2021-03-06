#ifndef IBLOCK_H
#define IBLOCK_H
// Created by Yifan

#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct iblock {
    char contents[4096]; // 4 * 1024
} iblock;
// directory[128], sizeof(directory) should == 4096, which is contents size
// when the inode is a dir, the block should contains a content, which has a directory

// not sure how many we need
void iblock_init(iblock* cur_block);
void iblock_free(iblock* iblock_ptr);
//int iblock_insert(void* cur_iblock, void* iblocks[], int iblock_bitmap[]);
int iblock_bitmap_find_next_empty(int* iblock_bitmap);
void** iblocks_addr();
int* iblock_bitmap_addr();
void* single_iblock_addr(int idx);
#endif

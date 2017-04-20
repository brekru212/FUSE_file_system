
#include <string.h>

#include "superblock.h"
#include "inode.h"
#include "iblock.h"
//import blocks
//import pages
//import inode

static superblock* sprblk;
void
superblock_init(void* disk_img)
{
    // offset for the superblock
    size_t offset = 0;

printf("making the superblock 1\n");
    // todo not sure if i need this
printf("size of superblock, disk_img: %d, %d\n",sizeof( sprblk), sizeof(disk_img));
   sprblk = disk_img;

printf("making the superblock 2\n");
    // offset for inode_bitmap
    offset += sizeof(superblock);
printf("making the superblock 2a: %d, %p\n", offset, sprblk);
    sprblk->ibitmap_location = (size_t*)offset;

printf("making the superblock 3\n"); fflush(stdout); fflush(stderr);
    // offset for iblock_bitmap
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->bbitmap_location = offset;

printf("making the superblock 4\n");
    // offset for inodes
    offset += 256 * sizeof(char); // use char => 1byte, int => 8bytes, saves space in "disk"
    sprblk->inodes = offset;

printf("making the superblock 5\n");
    // offset for iblocks
    offset += 256 * sizeof(inode);
    sprblk->iblocks = offset;

printf("making the superblock 6\n");
	sprblk->root_inode_idx = 0;
printf("finished making superblock");
}

void
superblock_free()
{
   // int rv = munmap(pages_base, NUFS_SIZE);
    // todo close?
   // assert(rv == 0);
}

void
print_superblock(superblock* superblock)
{/*
    if (node) {
        printf("node{refs: %d, mode: %04o, size: %d, xtra: %d}\n",
               node->refs, node->mode, node->size, node->xtra);
    }
    else {
        printf("node{null}\n");
    }*/
}


/* This si the function that looks for an empty space in a bitmap


int
find_empty_ibitmap() {
int empt = 0;

for (

}
*/

void
superblock_add_inode(const char* path) {
//look in bitmap for free location
//add inode to direcotry
//mark in bitmap that free location is used
}

superblock*
superblock_addr() {
    return (superblock*) sprblk;
}
/*
inode* inodes_addr() {
    return (inode*) (disk + sprblk->inodes);
}

int*
inode_bitmap_addr() {
    return (int*) (disk + sprblk->ibitmap_location);
}*/

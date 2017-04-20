#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

#include "storage.h"
#include "superblock.h"
#include "util.h"
//#include "util.h"


// implementation for: man 2 access
// Checks if a file exists.
int
nufs_access(const char *path, int mask)
{
    printf("access(%s, %04o)\n", path, mask); // debugging purpose
    // checks if the path exists
    int index = get_entry_index(path);
    if (index < 0) {
        return -ENOENT; // path doesn't exist
    }

    // get current user id
    int cur_uid = getuid();
    inode* cur_inode = inodes_addr()[index];
    // current user is not the owner
    if (cur_inode->user_id != cur_uid) {
//        if ()
        return -EACCES;
    }

    // Read, write, execute/search by owner

    // check u_id? return -EACCESS if the requested permission isn't available
//    struct stat* st;
//    int rv = nufs_getattr(path, st);
//    assert(rv == 0);
//
//
//
//    uid_t cur_uid = st->st_uid;
//    // Read, write, execute/search by owner
//    if ((mode == S_IRWXU) && (cur_uid != getuid())) {
//        return -EACCES;
//    }
    return 0; // success
}

// implementation for: man 2 stat
// gets an object's attributes (type, permissions, size, etc)
int
nufs_getattr(const char *path, struct stat *st)
{
    printf("In getattr(%s)\n", path); // debugging purpose
    int rv = get_stat(path, st);
    if (rv == -1) {
        return -ENOENT; // path doesn't exist
    }
    else {
        return 0;
    }
}

// implementation for: man 2 readdir
// lists the contents of a directory
// todo https://lastlog.de/misc/fuse-doc/doc/html/hello_8c.html
int
nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    struct stat st;

    printf("readdir(%s)\n", path);

    get_stat("/", &st);
    // filler is a callback that adds one item to the result
    // it will return non-zero when the buffer is full
    filler(buf, ".", &st, 0);

    get_stat("/hello.txt", &st);
    filler(buf, "hello.txt", &st, 0);

    return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
int
nufs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    // todo check if file already exist, if yes, return error

    int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    if (aval_idx < 0) {
        return -1; // ENOMEM: operation failed due to lack of memory or disk space
    }

    // create the new inode ptr
    inode* cur_inode = single_inode_addr(aval_idx);
    // flush the inode ptr to disk
    inodes_addr()[aval_idx] = cur_inode;
    inode_init(cur_inode, mode, 1, 0);
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // create the new iblock ptr
    iblock* cur_iblock = single_iblock_addr(aval_idx);
    // flush the iblock ptr to disk
    iblocks_addr()[aval_idx] = cur_iblock;
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    printf("after mknod(%s, %04o)\n", path, mode);
    return 0;
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int
nufs_mkdir(const char *path, mode_t mode)
{
    // todo check if dir already exist, if yes, return error
    int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    if (aval_idx < 0) {
        return -1; // ENOMEM: operation failed due to lack of memory or disk space
    }

    // create the new inode ptr
    inode* cur_inode = single_inode_addr(aval_idx);
    // flush the inode ptr to disk
    inodes_addr()[aval_idx] = cur_inode;
    inode_init(cur_inode, mode, 1, 0);
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // create the new iblock ptr
    directory* cur_dir = single_iblock_addr(aval_idx);

    directory_init(cur_dir, slist_last(path)->data);
    // flush the dir ptr to disk
    iblocks_addr()[aval_idx] = cur_dir;
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    printf("after mkdir(%s)\n", path);
    return -1;
}

int
nufs_unlink(const char *path)
{
    printf("unlink(%s)\n", path);
    return -1;
}

int
nufs_rmdir(const char *path)
{
    printf("rmdir(%s)\n", path);
    return -1;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int
nufs_rename(const char *from, const char *to)
{
    printf("rename(%s => %s)\n", from, to);
    return -1;
}

int
nufs_chmod(const char *path, mode_t mode)
{
    printf("chmod(%s, %04o)\n", path, mode);
    return -1;
}

int
nufs_truncate(const char *path, off_t size)
{
    printf("truncate(%s, %ld bytes)\n", path, size);
    return -1;
}

// this is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
int
nufs_open(const char *path, struct fuse_file_info *fi)
{
    printf("open(%s)\n", path);
    return 0;
}

// Actually read data
int
nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("read(%s, %ld bytes, @%ld)\n", path, size, offset);
    const char* data = get_data(path);

    int len = strlen(data) + 1;
    if (size < len) {
        len = size;
    }

    strlcpy(buf, data, len);
    return len;
}

// Actually write data
int
nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // todo inode_init
    printf("write(%s, %ld bytes, @%ld)\n", path, size, offset);
    return -1;
}

void
nufs_init_ops(struct fuse_operations* ops)
{
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access   = nufs_access;
    ops->getattr  = nufs_getattr;
    ops->readdir  = nufs_readdir;
    ops->mknod    = nufs_mknod;
    ops->mkdir    = nufs_mkdir;
    ops->unlink   = nufs_unlink;
    ops->rmdir    = nufs_rmdir;
    ops->rename   = nufs_rename;
    ops->chmod    = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open	  = nufs_open;
    ops->read     = nufs_read;
    ops->write    = nufs_write;
};

struct fuse_operations nufs_ops;

int
main(int argc, char *argv[])
{
    assert(argc > 2);
//	slist* path = split(argv[--argc]); - also call util function 
   storage_init(argv[--argc]); // disk_image
//   superblock_add_inode(argv[--argc]);
	 nufs_init_ops(&nufs_ops);
    // mount happened magically in fuse_main
    return fuse_main(argc, argv, &nufs_ops, NULL);
}

